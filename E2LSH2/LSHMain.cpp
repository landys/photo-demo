/*
 * Copyright (c) 2004-2005 Massachusetts Institute of Technology.
 * All Rights Reserved.
 *
 * MIT grants permission to use, copy, modify, and distribute this software and
 * its documentation for NON-COMMERCIAL purposes and without fee, provided that
 * this copyright notice appears in all copies.
 *
 * MIT provides this software "as is," without representations or warranties of
 * any kind, either expressed or implied, including but not limited to the
 * implied warranties of merchantability, fitness for a particular purpose, and
 * noninfringement.  MIT shall not be liable for any damages arising from any
 * use of this software.
 *
 * Author: Alexandr Andoni (andoni@mit.edu), Piotr Indyk (indyk@mit.edu)
 */

/*
  The main entry file containing the main() function. The main()
  function parses the command line parameters and depending on them
  calls the correspondin functions.
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "headers.h"
#include <string>
#include <algorithm>
#include <map>
#include <iostream>
#include <set>
#include <vector>
#include <utility>
//#include <Windows.h>
#include <cmath>
//#include <TCHAR.H>
#include "LSHMain.h"
using namespace std;

using std::map;
using std::pair;
using std::vector;
using std::sort;
using std::set;
using std::string;
#define N_SAMPLE_QUERY_POINTS 100

// The data set containing all the points.
PPointT *dataSetPoints = NULL;
// Number of points in the data set.
IntT nPoints = 0;

int pointsDimension = 128;

double parameterR = 50;

int parameterK = 16;

int parameterL = 40;


// The value of parameter R (a near neighbor of a point <q> is any
// point <p> from the data set that is the within distance
// <thresholdR>).
//RealT thresholdR = 1.0;

// The succes probability of each point (each near neighbor is
// reported by the algorithm with probability <successProbability>).
RealT successProbability = 0.9;

// Same as <thresholdR>, only an array of R's (for the case when
// multiple R's are specified).
RealT *listOfRadii = NULL;
IntT nRadii = 0;

RealT *memRatiosForNNStructs = NULL;

char sBuffer[600000];

/*
  Prints the usage of the LSHMain.
 */
void usage(char *programName){
  printf("Usage: %s #pts_in_data_set #queries dimension successProbability radius data_set_file query_points_file max_available_memory [-c|-p params_file]\n", programName);
}

inline PPointT readPoint(FILE *fileHandle){
  PPointT p;
  RealT sqrLength = 0;
  FAILIF(NULL == (p = (PPointT)MALLOC(sizeof(PointT))));
  FAILIF(NULL == (p->coordinates = (RealT*)MALLOC(pointsDimension * sizeof(RealT))));
  for(IntT d = 0; d < pointsDimension; d++){
    FSCANF_REAL(fileHandle, &(p->coordinates[d]));
    sqrLength += SQR(p->coordinates[d]);
  }
  fscanf(fileHandle, "%[^\n]", sBuffer);
  p->index = -1;
  p->sqrLength = sqrLength;
  return p;
}

// Reads in the data set points from <filename> in the array
// <dataSetPoints>. Each point get a unique number in the field
// <index> to be easily indentifiable.
void readDataSetFromFile(char *filename){
  FILE *f = fopen(filename, "rt");
  FAILIF(f == NULL);
  
  //fscanf(f, "%d %d ", &nPoints, &pointsDimension);
  //FSCANF_DOUBLE(f, &thresholdR);
  //FSCANF_DOUBLE(f, &successProbability);
  //fscanf(f, "\n");
  FAILIF(NULL == (dataSetPoints = (PPointT*)MALLOC(nPoints * sizeof(PPointT))));
  for(IntT i = 0; i < nPoints; i++){
    dataSetPoints[i] = readPoint(f);
    dataSetPoints[i]->index = i;
  }
  fclose(f);
}

// Tranforming <memRatiosForNNStructs> from
// <memRatiosForNNStructs[i]=ratio of mem/total mem> to
// <memRatiosForNNStructs[i]=ratio of mem/mem left for structs i,i+1,...>.
void transformMemRatios(){
  RealT sum = 0;
  for(IntT i = nRadii - 1; i >= 0; i--){
    sum += memRatiosForNNStructs[i];
    memRatiosForNNStructs[i] = memRatiosForNNStructs[i] / sum;
    //DPRINTF("%0.6lf\n", memRatiosForNNStructs[i]);
  }
  ASSERT(sum <= 1.000001);
}


int compareInt32T(const void *a, const void *b){
  Int32T *x = (Int32T*)a;
  Int32T *y = (Int32T*)b;
  return (*x > *y) - (*x < *y);
}

/*
  The main entry to LSH package. Depending on the command line
  parameters, the function computes the R-NN data structure optimal
  parameters and/or construct the R-NN data structure and runs the
  queries on the data structure.
 */
int old_main(int nargs, char **args){
  if(nargs < 9){
    usage(args[0]);
    exit(1);
  }

  //initializeLSHGlobal();

  // Parse part of the command-line parameters.
  nPoints = atoi(args[1]);
  IntT nQueries = atoi(args[2]);
  pointsDimension = atoi(args[3]);
  successProbability = atof(args[4]);
  char* endPtr[1];
  RealT thresholdR = strtod(args[5], endPtr);
  if (thresholdR == 0 || endPtr[1] == args[5]){
    // The value for R is not specified, instead there is a file
    // specifying multiple R's.
    thresholdR = 0;

    // Read in the file
    FILE *radiiFile = fopen(args[5], "rt");
    FAILIF(radiiFile == NULL);
    fscanf(radiiFile, "%d\n", &nRadii);
    ASSERT(nRadii > 0);
    FAILIF(NULL == (listOfRadii = (RealT*)MALLOC(nRadii * sizeof(RealT))));
    FAILIF(NULL == (memRatiosForNNStructs = (RealT*)MALLOC(nRadii * sizeof(RealT))));
    for(IntT i = 0; i < nRadii; i++){
      FSCANF_REAL(radiiFile, &listOfRadii[i]);
      ASSERT(listOfRadii[i] > 0);
      FSCANF_REAL(radiiFile, &memRatiosForNNStructs[i]);
      ASSERT(memRatiosForNNStructs[i] > 0);
    }
  }else{
    nRadii = 1;
    FAILIF(NULL == (listOfRadii = (RealT*)MALLOC(nRadii * sizeof(RealT))));
    FAILIF(NULL == (memRatiosForNNStructs = (RealT*)MALLOC(nRadii * sizeof(RealT))));
    listOfRadii[0] = thresholdR;
    memRatiosForNNStructs[0] = 1;
  }
  DPRINTF("No. radii: %d\n", nRadii);
  //thresholdR = atof(args[5]);
  availableTotalMemory = atol(args[8]);

  if (nPoints > MAX_N_POINTS) {
    printf("Error: the structure supports at most %d points (%d were specified).\n", MAX_N_POINTS, nPoints);
    fprintf(ERROR_OUTPUT, "Error: the structure supports at most %d points (%d were specified).\n", MAX_N_POINTS, nPoints);
    exit(1);
  }

  readDataSetFromFile(args[6]);
  DPRINTF("Allocated memory (after reading data set): %d\n", totalAllocatedMemory);

  Int32T nSampleQueries = N_SAMPLE_QUERY_POINTS;
  //PPointT sampleQueries[nSampleQueries];
  PPointT* sampleQueries = (PPointT*) alloca(sizeof(PPointT) * nSampleQueries);
  //Int32T sampleQBoundaryIndeces[nSampleQueries];
  Int32T* sampleQBoundaryIndeces = (Int32T*) alloca(sizeof(Int32T) * nSampleQueries);
  if ((nargs < 9) || (strcmp("-c", args[9]) == 0)){
    // In this cases, we need to generate a sample query set for
    // computing the optimal parameters.

    // Generate a sample query set.
    FILE *queryFile = fopen(args[7], "rt");
    if (strcmp(args[7], ".") == 0 || queryFile == NULL || nQueries <= 0){
      // Choose several data set points for the sample query points.
      for(IntT i = 0; i < nSampleQueries; i++){
	sampleQueries[i] = dataSetPoints[genRandomInt(0, nPoints - 1)];
      }
    }else{
      // Choose several actual query points for the sample query points.
      nSampleQueries = MIN(nSampleQueries, nQueries);
      //Int32T sampleIndeces[nSampleQueries];
	  Int32T* sampleIndeces = (Int32T*) alloca(sizeof(Int32T) * nSampleQueries);
      for(IntT i = 0; i < nSampleQueries; i++){
	sampleIndeces[i] = genRandomInt(0, nQueries - 1);
      }
      qsort(sampleIndeces, nSampleQueries, sizeof(*sampleQueries), compareInt32T);
      //printIntVector("sampleIndeces: ", nSampleQueries, sampleIndeces);
      Int32T j = 0;
      for(Int32T i = 0; i < nQueries; i++){
	if (i == sampleIndeces[j]){
	  sampleQueries[j] = readPoint(queryFile);
	  j++;
	  while (i == sampleIndeces[j]){
	    sampleQueries[j] = sampleQueries[j - 1];
	    j++;
	  }
	}else{
	  fscanf(queryFile, "%[^\n]", sBuffer);
	  fscanf(queryFile, "\n");
	}
      }
      nSampleQueries = j;
      fclose(queryFile);
    }

    // Compute the array sampleQBoundaryIndeces that specifies how to
    // segregate the sample query points according to their distance
    // to NN.
    sortQueryPointsByRadii(pointsDimension,
			   nSampleQueries,
			   sampleQueries,
			   nPoints,
			   dataSetPoints,
			   nRadii,
			   listOfRadii,
			   sampleQBoundaryIndeces);
  }

  RNNParametersT *algParameters = NULL;
  PRNearNeighborStructT *nnStructs = NULL;
  if (nargs > 9) {
    // Additional command-line parameter is specified.
    if (strcmp("-c", args[9]) == 0) {
      // Only compute the R-NN DS parameters and output them to stdout.
      
      printf("%d\n", nRadii);
      transformMemRatios();
      for(IntT i = 0; i < nRadii; i++){
	// which sample queries to use
	Int32T segregatedQStart = (i == 0) ? 0 : sampleQBoundaryIndeces[i - 1];
	Int32T segregatedQNumber = nSampleQueries - segregatedQStart;
	if (segregatedQNumber == 0) {
	  // XXX: not the right answer
	  segregatedQNumber = nSampleQueries;
	  segregatedQStart = 0;
	}
	ASSERT(segregatedQStart < nSampleQueries);
	ASSERT(segregatedQStart >= 0);
	ASSERT(segregatedQStart + segregatedQNumber <= nSampleQueries);
	ASSERT(segregatedQNumber >= 0);
	RNNParametersT optParameters = computeOptimalParameters(listOfRadii[i],
								successProbability,
								nPoints,
								pointsDimension,
								dataSetPoints,
								segregatedQNumber,
								sampleQueries + segregatedQStart,
								(Uns32T)((availableTotalMemory - totalAllocatedMemory) * memRatiosForNNStructs[i]));
	printRNNParameters(stdout, optParameters);
      }
      exit(0);
    } else if (strcmp("-p", args[9]) == 0) {
      // Read the R-NN DS parameters from the given file and run the
      // queries on the constructed data structure.
      if (nargs < 10){
	usage(args[0]);
	exit(1);
      }
      FILE *pFile = fopen(args[10], "rt");
      FAILIFWR(pFile == NULL, "Could not open the params file.");
      fscanf(pFile, "%d\n", &nRadii);
      DPRINTF1("Using the following R-NN DS parameters:\n");
      DPRINTF("N radii = %d\n", nRadii);
      FAILIF(NULL == (nnStructs = (PRNearNeighborStructT*)MALLOC(nRadii * sizeof(PRNearNeighborStructT))));
      FAILIF(NULL == (algParameters = (RNNParametersT*)MALLOC(nRadii * sizeof(RNNParametersT))));
      for(IntT i = 0; i < nRadii; i++){
	algParameters[i] = readRNNParameters(pFile);
	printRNNParameters(stderr, algParameters[i]);
	nnStructs[i] = initLSH_WithDataSet(algParameters[i], nPoints, dataSetPoints);
      }

      pointsDimension = algParameters[0].dimension;
      FREE(listOfRadii);
      FAILIF(NULL == (listOfRadii = (RealT*)MALLOC(nRadii * sizeof(RealT))));
      for(IntT i = 0; i < nRadii; i++){
	listOfRadii[i] = algParameters[i].parameterR;
      }
    } else{
      // Wrong option.
      usage(args[0]);
      exit(1);
    }
  } else {
    FAILIF(NULL == (nnStructs = (PRNearNeighborStructT*)MALLOC(nRadii * sizeof(PRNearNeighborStructT))));
    // Determine the R-NN DS parameters, construct the DS and run the queries.
    transformMemRatios();
    for(IntT i = 0; i < nRadii; i++){
      // XXX: segregate the sample queries...
      nnStructs[i] = initSelfTunedRNearNeighborWithDataSet(listOfRadii[i], 
							   successProbability, 
							   nPoints, 
							   pointsDimension, 
							   dataSetPoints, 
							   nSampleQueries, 
							   sampleQueries, 
							   (Uns32T)((availableTotalMemory - totalAllocatedMemory) * memRatiosForNNStructs[i]));
    }
  }

  DPRINTF1("X\n");

  IntT resultSize = nPoints;
  PPointT *result = (PPointT*)MALLOC(resultSize * sizeof(*result));
  PPointT queryPoint;
  FAILIF(NULL == (queryPoint = (PPointT)MALLOC(sizeof(PointT))));
  FAILIF(NULL == (queryPoint->coordinates = (RealT*)MALLOC(pointsDimension * sizeof(RealT))));

  FILE *queryFile = fopen(args[7], "rt");
  FAILIF(queryFile == NULL);
  TimeVarT meanQueryTime = 0;
  for(IntT i = 0; i < nQueries; i++){

    RealT sqrLength = 0;
    // read in the query point.
    for(IntT d = 0; d < pointsDimension; d++){
      FSCANF_REAL(queryFile, &(queryPoint->coordinates[d]));
      sqrLength += SQR(queryPoint->coordinates[d]);
    }
    queryPoint->sqrLength = sqrLength;
    //printRealVector("Query: ", pointsDimension, queryPoint->coordinates);

    // get the near neighbors.
    IntT nNNs = 0;
    for(IntT r = 0; r < nRadii; r++){
      nNNs = getRNearNeighbors(nnStructs[r], queryPoint, result, resultSize);
      printf("Total time for R-NN query at radius %0.6lf (radius no. %d):\t%0.6lf\n", (double)(listOfRadii[r]), r, timeRNNQuery);
      meanQueryTime += timeRNNQuery;

      if (nNNs > 0){
	printf("Query point %d: found %d NNs at distance %0.6lf (radius no. %d). NNs are:\n", i, nNNs, (double)(listOfRadii[r]), r);
	for(IntT j = 0; j < nNNs; j++){
	  ASSERT(result[j] != NULL);
	  printf("%09d\tdist:%0.6lf\n", result[j]->index, distance(pointsDimension, queryPoint, result[j]));
	  CR_ASSERT(distance(pointsDimension, queryPoint, result[j]) <= listOfRadii[r]);
	  //DPRINTF("Distance: %lf\n", distance(pointsDimension, queryPoint, result[j]));
	  //printRealVector("NN: ", pointsDimension, result[j]->coordinates);
	}
	break;
      }
    }
    if (nNNs == 0){
      printf("Query point %d: no NNs found.\n", i);
    }
  }
  if (nQueries > 0){
    meanQueryTime = meanQueryTime / nQueries;
    printf("Mean query time: %0.6lf\n", (double)meanQueryTime);
  }

  //freePRNearNeighborStruct(nnStruct);

  return 0;
}


// 以下内容为项目中添加的内容

// 用到的一些常量
// 保存到文件
const int IGNORE_DIMENSION = 4;
const int MAX_IN_ONE_BUCKET = 200;
const int MAX_POINTS_IN_ONE_PASS = 20000;
const int BUCKET_NUM = 50000;
const int HASH_TABLE_SIZE = BUCKET_NUM;
const int MAX_FILE_NAME_LENGTH = 255;
const int MIN_MATCH_GAP = 5;
const int ONE_ELEMENT = sizeof(int) + sizeof(Uns32T);
const int ONE_ROW = (sizeof(int) + sizeof(Uns32T)) * MAX_IN_ONE_BUCKET + sizeof(int);
const int DATASET_ONE_ROW_SIZE = MAX_FILE_NAME_LENGTH * sizeof(char) + sizeof(double) * (pointsDimension + IGNORE_DIMENSION);

void setUpIndexFromDataSet(string dataSetFileName, string indexFileName, RNNParametersT optParameters);
void addOnePointToBucket(FILE *fp, int bucketIndex, int pointIndex,Uns32T hValue);
void initAllDiskBucket(FILE* fp, string dataFileName);
void queryInner(string queryFileName, string indexFileName, RNNParametersT optParameters, string outputFile);
PRNearNeighborStructT getHashedStructure(RNNParametersT algParameters, bool isFirst, bool useFileToInit, char* index);
PRNearNeighborStructT getHashedStructure(RNNParametersT algParameters, bool isFirst, bool useFileToInit, FILE* index);
void saveHashfunction(PRNearNeighborStructT nnStruct, char* indexFile);
void saveHashfunction(PRNearNeighborStructT nnStruct, FILE* indexFile);
int* getBucketFromDisk(FILE* fp, int hIndex, Uns32T hValue, int& bucketSize);
PPointT getPointFromDataset(FILE* datasetFile, int index);
string getPictureId(FILE* fp, int index);
double computeAlpha(string indexFileName);
void outputIndexFile(string indexFileName, string outputTextFileName);

/*
The main entry to LSH package. Depending on the command line
parameters, the function computes the R-NN data structure optimal
parameters and/or construct the R-NN data structure and runs the
queries on the data structure.
*/
int main(int nargs, char **args){
	RNNParametersT optParameters;
	optParameters.successProbability = successProbability;
	optParameters.dimension = pointsDimension;
	optParameters.parameterR = parameterR;
	optParameters.parameterR2 = SQR(optParameters.parameterR);
	optParameters.parameterW = PARAMETER_W_DEFAULT;
	optParameters.typeHT = HT_HYBRID_CHAINS;
	optParameters.parameterK = parameterK;
	optParameters.parameterL = parameterL;
	optParameters.parameterM = computeLfromKP(optParameters.parameterK, optParameters.successProbability);
	optParameters.useUfunctions = false;

	//setUpIndex("keypointIndex", "index3", 0, 0, 40, 0);

	//printf("%lf\n", computeAlpha("index36"));
	//outputIndexFile("index36", "index.txt");
	query("E:\\projects\\photodemo\\codes\\trunk\\bin\\Debug\\data\\keypointMatch", 
		"E:\\projects\\photodemo\\codes\\trunk\\bin\\Debug\\data\\indexes\\index2", 
		"E:\\projects\\photodemo\\codes\\trunk\\bin\\Debug\\data\\output.txt");
	//queryInner("keypointMatch", "index36", optParameters, "output.txt");
}

/*extern "C" __declspec(dllexport) int test() {
	printf("%d", 1);
	return 1;
}*/
/*
extern "C" __declspec(dllexport) void setUpIndex(char* dataFileStr, char* indexNameStr) {
	
    RNNParametersT optParameters;
	optParameters.successProbability = successProbability;
	optParameters.dimension = pointsDimension;
	optParameters.parameterR = parameterR;
	optParameters.parameterR2 = SQR(optParameters.parameterR);
	optParameters.parameterW = PARAMETER_W_DEFAULT;
	optParameters.typeHT = HT_HYBRID_CHAINS;
	optParameters.parameterK = parameterK;
	optParameters.parameterL = parameterL;
	optParameters.parameterM = computeLfromKP(optParameters.parameterK, optParameters.successProbability);
	optParameters.useUfunctions = false;

	//FILE* fp = fopen("log", "w+t");
	//fprintf(fp, "Starting to set up the index with %s to %s.\n", dataFileStr, indexNameStr);
	//fclose(fp);
	string dataFile(dataFileStr), indexName(indexNameStr);
	setUpIndexFromDataSet(dataFile, indexName, optParameters);
}
*/

void saveParameter(string indexName, double inputR, double inputW, int inputK, int inputL) {
	FILE* indexFile = fopen(indexName.c_str(), "a+b");
	
	fseek(indexFile, 0, SEEK_END);
	fwrite(&inputR, sizeof(inputR), 1, indexFile);
	fwrite(&inputW, sizeof(inputW), 1, indexFile);
	fwrite(&inputK, sizeof(inputK), 1, indexFile);
	fwrite(&inputL, sizeof(inputL), 1, indexFile);
	fclose(indexFile);
}
#ifdef WIN32
extern "C" __declspec(dllexport) void setUpIndex(char* dataFileStr, char* indexNameStr, double inputR/* = parameterR*/, double inputW/* = PARAMETER_W_DEFAULT*/, int inputK/* = parameterK*/, int inputL/* = parameterL*/) {
#else
extern "C" void setUpIndex(char* dataFileStr, char* indexNameStr, double inputR/* = parameterR*/, double inputW/* = PARAMETER_W_DEFAULT*/, int inputK/* = parameterK*/, int inputL/* = parameterL*/) {
#endif // WIN32
    RNNParametersT optParameters;

	// 设置默认参数
	if (fabs(inputR) < 1e-8) {
		inputR = parameterR;
	}
	if (fabs(inputW) < 1e-8) {
		inputW = PARAMETER_W_DEFAULT;
	}
	if (inputK == 0) {
		inputK = parameterK;
	}
	if (inputL == 0) {
		inputL = parameterL;
	}

	optParameters.successProbability = successProbability;
	optParameters.dimension = pointsDimension;
	optParameters.parameterR = inputR;
	optParameters.parameterR2 = SQR(optParameters.parameterR);
	optParameters.parameterW = inputW;
	optParameters.typeHT = HT_HYBRID_CHAINS;
	optParameters.parameterK = inputK;
	optParameters.parameterL = inputL;
	optParameters.parameterM = computeLfromKP(optParameters.parameterK, optParameters.successProbability);
	optParameters.useUfunctions = false;

	//FILE* fp = fopen("log", "w+t");
	//fprintf(fp, "Starting to set up the index with %s to %s.\n", dataFileStr, indexNameStr);
	//fclose(fp);
	string dataFile(dataFileStr), indexName(indexNameStr);
	setUpIndexFromDataSet(dataFile, indexName, optParameters);
	saveParameter(indexName, inputR, inputW, inputK, inputL);
}

void getParameter(string indexName, double& inputR, double& inputW, int& inputK, int& inputL) {
	FILE* indexFile = fopen(indexName.c_str(), "rb");

	fseek(indexFile, -((int) sizeof(inputR) + (int) sizeof(inputW) + (int) sizeof(inputK) + (int) sizeof(inputL)), SEEK_END);
	fread(&inputR, sizeof(inputR), 1, indexFile);
	fread(&inputW, sizeof(inputW), 1, indexFile);
	fread(&inputK, sizeof(inputK), 1, indexFile);
	fread(&inputL, sizeof(inputL), 1, indexFile);
	fclose(indexFile);
}

#ifdef WIN32
extern "C" __declspec(dllexport) void query(char* queryFileStr, char* indexNameStr, char* outputFileStr) {
#else
extern "C" void query(char* queryFileStr, char* indexNameStr, char* outputFileStr) {
#endif // WIN32
	string queryFile(queryFileStr);
	string indexName(indexNameStr);
	string outputFile(outputFileStr);

	double inputR, inputW;
	int inputK, inputL;
	getParameter(indexName, inputR, inputW, inputK, inputL);
    RNNParametersT optParameters;
	optParameters.successProbability = successProbability;
	optParameters.dimension = pointsDimension;
	optParameters.parameterR = inputR;
	optParameters.parameterR2 = SQR(optParameters.parameterR);
	optParameters.parameterW = inputW;
	optParameters.typeHT = HT_HYBRID_CHAINS;
	optParameters.parameterK = inputK;
	optParameters.parameterL = inputL;
	optParameters.parameterM = computeLfromKP(optParameters.parameterK, optParameters.successProbability);
	optParameters.useUfunctions = false;

	queryInner(queryFile, indexName, optParameters, outputFile);
}

void outputIndexFile(string indexFileName, string outputTextFileName) {
	FILE *fp = fopen(indexFileName.c_str(), "rb");
	FILE *output = fopen(outputTextFileName.c_str(), "w+t");

	fread(sBuffer, sizeof(char), MAX_FILE_NAME_LENGTH, fp);

	fprintf(output, "%s\n", sBuffer);
	int cnt = 0;
	for (int i = 0; i < BUCKET_NUM; i++) {
		printf("Output bucket %d\n.", i);
		int num;
		fread(&num, sizeof(num), 1, fp);
	
		fprintf(output, "%3d", num);

		for (int j = 0; j < num; j++) {
			int index;
			unsigned int value;
			fread(&index, sizeof(index), 1, fp);
			fread(&value, sizeof(value), 1, fp);

			fprintf(output, " (%6d, %6u)", index, value);
		}

		fseek(fp, ONE_ELEMENT * (MAX_IN_ONE_BUCKET - num), SEEK_CUR);
		fprintf(output, "\n");
	}

	fclose(fp);
	fclose(output);
}

// 把数据文件由文本转化为二进制
// 原来的文件"aaa"会被转存为二进制文件"aaa.bin"
void convertTextDataFile2BinFile(string textDataFileName) {
	FILE* textFile = fopen(textDataFileName.c_str(), "rt");
	FILE* binFile = fopen((textDataFileName + ".bin").c_str(), "w+b");

	while (true) {
		fscanf(textFile, "%s", sBuffer);

		int ret = fwrite(sBuffer, sizeof(char), MAX_FILE_NAME_LENGTH, binFile);

		IntT d = 0;
		for(d = 0; d < pointsDimension + IGNORE_DIMENSION; d++){
			double current = 0;

			
			int ret = fscanf(textFile, "%lf", &current);

			if (ret == EOF) {
				// end of file.
				break;
			} else if (ret != 1) {
				break;
			}
			
			fwrite(&current, sizeof(double), 1, binFile);
		}

		if (d != pointsDimension + IGNORE_DIMENSION) {
			break;
		}
	}

	fclose(textFile);
	fclose(binFile);
}
/// data
///
void setUpIndexFromDataSet(string dataSetFileName, string indexFileName, RNNParametersT optParameters)
{
    convertTextDataFile2BinFile(dataSetFileName);

	FILE *datasetFile = fopen((dataSetFileName + ".bin").c_str(), "rb");
	FILE *indexFile =fopen(indexFileName.c_str(), "w+b");

	FAILIF(datasetFile == NULL);
	FAILIF(indexFile == NULL);

	bool isFirst = true;
	int cnt = 0;

	PRNearNeighborStructT nnStruct = getHashedStructure(optParameters, isFirst, false, indexFile);
	
	nnStruct->useUfunctions = false;
	//初始化文件表
	initAllDiskBucket(indexFile, dataSetFileName + ".bin");

	Uns32T *mainHashA = NULL, *controlHash1 = NULL;

	while (true) {
		// 每次处理一定数目的输入数据点
		FAILIF(NULL == (dataSetPoints = (PPointT*)MALLOC(MAX_POINTS_IN_ONE_PASS * sizeof(PPointT))));

		IntT pointsCurrent = 0;

		for(IntT i = 0; i < MAX_POINTS_IN_ONE_PASS; i++){
			PPointT p;
			RealT sqrLength = 0;
			
			FAILIF(NULL == (p = (PPointT)MALLOC(sizeof(PointT))));
			FAILIF(NULL == (p->coordinates = (RealT*)MALLOC(pointsDimension * sizeof(RealT))));
			
			bool isEnd = false;
			fread(sBuffer, sizeof(char), MAX_FILE_NAME_LENGTH, datasetFile);
			//fscanf(datasetFile, "%s", sBuffer);
			int ignore = IGNORE_DIMENSION;
			for(IntT d = 0; d < pointsDimension; d++){
				//if (fscanf(datasetFile, "%lf", &(p->coordinates[d])) == EOF) {
				if (fread(&(p->coordinates[d]), sizeof(double), 1, datasetFile) != 1) {
					FAILIF(d != 0);
					isEnd = true;
					break;
				}
				sqrLength += SQR(p->coordinates[d]);

				if (ignore > 0) ignore--, d--;
			}

			p->index = -1;
			p->sqrLength = sqrLength;

			dataSetPoints[i] = p;
			dataSetPoints[i]->index = i;
			pointsCurrent++;

			if (isEnd) {
				pointsCurrent--;
				FREE(p->coordinates);
				FREE(p);
				break;
			}
		}

		if (pointsCurrent == 0){
			break;
		}

		// 得到LSH参数结构
		optParameters.parameterT = pointsCurrent;
		PRNearNeighborStructT *nnStructs = NULL;


		ASSERT(optParameters.typeHT == HT_HYBRID_CHAINS);
		ASSERT(dataSetPoints != NULL);
		ASSERT(USE_SAME_UHASH_FUNCTIONS);

		//printRNNParameters(stderr, optParameters);
		printf("Started handling points starting from %d.\n", cnt);

		// Set the fields <nPoints> and <points>.
		nnStruct->nPoints = pointsCurrent;
		for(Int32T i = 0; i < pointsCurrent; i++){
			nnStruct->points[i] = dataSetPoints[i];
		}

		// initialize second level hashing (bucket hashing)
		FAILIF(NULL == (nnStruct->hashedBuckets = (PUHashStructureT*)MALLOC(nnStruct->parameterL * sizeof(PUHashStructureT))));

		PUHashStructureT modelHT = NULL;
		
		if (isFirst) {
			modelHT = newUHashStructure(HT_LINKED_LIST, HASH_TABLE_SIZE, nnStruct->parameterK, FALSE, mainHashA, controlHash1, NULL);
		} else {
			modelHT = newUHashStructure(HT_LINKED_LIST, HASH_TABLE_SIZE, nnStruct->parameterK, TRUE, mainHashA, controlHash1, NULL);
			modelHT->mainHashA = mainHashA;
			modelHT->controlHash1 = controlHash1;
		}


		// Uns32T **(precomputedHashesOfULSHs[nnStruct->nHFTuples]);
		Uns32T *** precomputedHashesOfULSHs = (Uns32T ***) alloca(nnStruct->nHFTuples * sizeof(Uns32T**));
		for(IntT l = 0; l < nnStruct->nHFTuples; l++){
			FAILIF(NULL == (precomputedHashesOfULSHs[l] = (Uns32T**)MALLOC(pointsCurrent * sizeof(Uns32T*))));
			for(IntT i = 0; i < pointsCurrent; i++){
				FAILIF(NULL == (precomputedHashesOfULSHs[l][i] = (Uns32T*)MALLOC(N_PRECOMPUTED_HASHES_NEEDED * sizeof(Uns32T))));
			}
		}

		for(IntT i = 0; i < pointsCurrent; i++){
			preparePointAdding(nnStruct, modelHT, dataSetPoints[i]);
			for(IntT l = 0; l < nnStruct->nHFTuples; l++){
#ifdef MYDEBUG
				if (i == 0 && l < 10) printf("%d:  ", l);
#endif
				for(IntT h = 0; h < N_PRECOMPUTED_HASHES_NEEDED; h++){
					precomputedHashesOfULSHs[l][i][h] = nnStruct->precomputedHashesOfULSHs[l][h];
#ifdef MYDEBUG
					if (i == 0 && l < 10) printf(" %d", precomputedHashesOfULSHs[l][i][h]);
#endif
				}
#ifdef MYDEBUG
				if (i == 0 && l < 10) printf("\n");
#endif
			}
		}

		//DPRINTF("Allocated memory(modelHT and precomputedHashesOfULSHs just a.): %d\n", totalAllocatedMemory);

		
		// Initialize the counters for defining the pair of <u> functions used for <g> functions.

		IntT firstUComp = 0;
		IntT secondUComp = 1;
		vector<pair<int, pair<int, int> > > prepareInsert;
		for(IntT i = 0; i < nnStruct->parameterL; i++){
			// build the model HT.
			for(IntT p = 0; p < pointsCurrent; p++){
				// Add point <dataSet[p]> to modelHT.
				if (!nnStruct->useUfunctions) {
					// Use usual <g> functions (truly independent; <g>s are precisly
					// <u>s).
					addBucketEntry(modelHT, 1, precomputedHashesOfULSHs[i][p], NULL, cnt + p);
				} else {
					// Use <u> functions (<g>s are pairs of <u> functions).
					addBucketEntry(modelHT, 2, precomputedHashesOfULSHs[firstUComp][p], precomputedHashesOfULSHs[secondUComp][p], cnt + p);
				}
			}
#ifdef MYDEBUG
			printf("\n");
#endif
			//ASSERT(nAllocatedGBuckets <= nPoints);
			//ASSERT(nAllocatedBEntries <= nPoints);

			// compute what is the next pair of <u> functions.
			secondUComp++;
			if (secondUComp == nnStruct->nHFTuples) {
				firstUComp++;
				secondUComp = firstUComp + 1;
			}

			// copy the model HT into the actual (packed) HT. copy the uhash function too.
			nnStruct->hashedBuckets[i] = newUHashStructure(optParameters.typeHT, HASH_TABLE_SIZE, nnStruct->parameterK, TRUE, mainHashA, controlHash1, modelHT);

			for (int j = 0; j < modelHT->hashTableSize; j++) {
				PGBucketT p = modelHT->hashTable.llHashTable[j];

				if (p != NULL)
				{
					PBucketEntryT bucketEntry = &p->firstEntry;

					while (bucketEntry != NULL)
					{
						prepareInsert.push_back(make_pair(j, make_pair(bucketEntry->pointIndex, bucketEntry->hashValue)));
						//addOnePointToBucket(indexFile, j, bucketEntry->pointIndex, bucketEntry->hashValue);
						bucketEntry = bucketEntry->nextEntry;
					}
				}
			}

			// clear the model HT for the next iteration.
			clearUHashStructure(modelHT);
		}

		sort(prepareInsert.begin(), prepareInsert.end());

		for (int i = 0; i < prepareInsert.size(); i++) {
			addOnePointToBucket(indexFile, prepareInsert[i].first, prepareInsert[i].second.first, prepareInsert[i].second.second);
		}
		if (isFirst) {
			saveHashfunction(nnStruct, indexFile);
		}

		for (int i = 0; i < pointsCurrent; i++) {
			FREE(dataSetPoints[i]->coordinates);
			FREE(dataSetPoints[i]);
		}

		FREE(dataSetPoints);
		freeUHashStructure(modelHT, FALSE); // do not free the uhash functions since they are used by nnStruct->hashedBuckets[i]

		// freeing precomputedHashesOfULSHs
		for(IntT l = 0; l < nnStruct->nHFTuples; l++){
			for(IntT i = 0; i < pointsCurrent; i++){
				FREE(precomputedHashesOfULSHs[l][i]);
			}
			FREE(precomputedHashesOfULSHs[l]);
		}
		

		for(IntT i = 0; i < nnStruct->parameterL; i++) {
			FREE(nnStruct->hashedBuckets[i]->hashTable.hybridHashTable);
			FREE(nnStruct->hashedBuckets[i]->hybridChainsStorage);

			FREE(nnStruct->hashedBuckets[i]);
		}
		FREE(nnStruct->hashedBuckets);
		isFirst = false;

		cnt += pointsCurrent;
	}
	
	FREE(nnStruct);
	fclose(datasetFile);
	fclose(indexFile);
}

void saveHashfunction(PRNearNeighborStructT nnStruct, FILE* indexFile)
{
	fseek(indexFile, MAX_FILE_NAME_LENGTH * sizeof(char) + ONE_ROW * BUCKET_NUM, SEEK_SET);

	LSHFunctionT **lshFunctions = nnStruct->lshFunctions;
	// allocate memory for the functions
	// initialize the LSH functions
	for(IntT i = 0; i < nnStruct->nHFTuples; i++){
		for(IntT j = 0; j < nnStruct->hfTuplesLength; j++){
			// vector a
			for(IntT d = 0; d < nnStruct->dimension; d++){
#ifdef USE_L1_DISTANCE
				lshFunctions[i][j].a[d] = getCauchyRandom();
#else
				fwrite(&lshFunctions[i][j].a[d], sizeof(lshFunctions[i][j].a[d]), 1, indexFile);
#endif
			}
			// b
			fwrite(&lshFunctions[i][j].b, sizeof(lshFunctions[i][j].b), 1, indexFile);
		}
	}

	for (int i = 0; i < nnStruct->parameterL; i++) {
		for (int j = 0; j < nnStruct->hashedBuckets[i]->hashedDataLength; j++) {
			fwrite(&nnStruct->hashedBuckets[i]->mainHashA[j], sizeof(nnStruct->hashedBuckets[i]->mainHashA[j]), 1, indexFile);
			fwrite(&nnStruct->hashedBuckets[i]->controlHash1[j], sizeof(nnStruct->hashedBuckets[i]->controlHash1[j]), 1, indexFile);
		}
	}

	nnStruct->lshFunctions = lshFunctions;
}

void getHashfunction(PRNearNeighborStructT nnStruct, char* indexFile)
{
	char* current = indexFile + MAX_FILE_NAME_LENGTH * sizeof(char) + ONE_ROW * BUCKET_NUM;
	LSHFunctionT **lshFunctions;
	// allocate memory for the functions
	FAILIF(NULL == (lshFunctions = (LSHFunctionT**)MALLOC(nnStruct->nHFTuples * sizeof(LSHFunctionT*))));
	for(IntT i = 0; i < nnStruct->nHFTuples; i++){
		FAILIF(NULL == (lshFunctions[i] = (LSHFunctionT*)MALLOC(nnStruct->hfTuplesLength * sizeof(LSHFunctionT))));
		for(IntT j = 0; j < nnStruct->hfTuplesLength; j++){
			FAILIF(NULL == (lshFunctions[i][j].a = (RealT*)MALLOC(nnStruct->dimension * sizeof(RealT))));
		}
	}

	// initialize the LSH functions
	for(IntT i = 0; i < nnStruct->nHFTuples; i++){
		for(IntT j = 0; j < nnStruct->hfTuplesLength; j++){
			// vector a
			for(IntT d = 0; d < nnStruct->dimension; d++){
#ifdef USE_L1_DISTANCE
				lshFunctions[i][j].a[d] = getCauchyRandom();
#else
				memcpy(&lshFunctions[i][j].a[d], current, sizeof(lshFunctions[i][j].a[d]));
				current += sizeof(lshFunctions[i][j].a[d]);
#endif
			}
			// b
			memcpy(&lshFunctions[i][j].b, current, sizeof(lshFunctions[i][j].b));
			current += sizeof(lshFunctions[i][j].b);
		}
	}

	FAILIF(NULL == (nnStruct->hashedBuckets = (PUHashStructureT*)MALLOC(nnStruct->parameterL * sizeof(PUHashStructureT))));
	for (int i = 0; i < nnStruct->parameterL; i++) {
		FAILIF(NULL == (nnStruct->hashedBuckets[i] = (UHashStructureT *)MALLOC(nnStruct->parameterL * sizeof(UHashStructureT))));
		nnStruct->hashedBuckets[i]->hashedDataLength = nnStruct->parameterK;
		FAILIF(NULL == (nnStruct->hashedBuckets[i]->mainHashA = (unsigned int*)MALLOC(nnStruct->parameterK * sizeof(unsigned int))));
		FAILIF(NULL == (nnStruct->hashedBuckets[i]->controlHash1 = (unsigned int*)MALLOC(nnStruct->parameterK * sizeof(unsigned int))));
		for (int j = 0; j < nnStruct->parameterK; j++) {
			memcpy(&nnStruct->hashedBuckets[i]->mainHashA[j], current, sizeof(nnStruct->hashedBuckets[i]->mainHashA[j]));
			current += sizeof(nnStruct->hashedBuckets[i]->mainHashA[j]);
			memcpy(&nnStruct->hashedBuckets[i]->controlHash1[j], current, sizeof(nnStruct->hashedBuckets[i]->controlHash1[j]));
			current += sizeof(nnStruct->hashedBuckets[i]->controlHash1[j]);
		}
	}

	nnStruct->lshFunctions = lshFunctions;
}

void getHashfunction(PRNearNeighborStructT nnStruct, FILE* indexFile)
{
	fseek(indexFile, MAX_FILE_NAME_LENGTH * sizeof(char) + ONE_ROW * BUCKET_NUM, SEEK_SET);
	LSHFunctionT **lshFunctions;
	// allocate memory for the functions
	FAILIF(NULL == (lshFunctions = (LSHFunctionT**)MALLOC(nnStruct->nHFTuples * sizeof(LSHFunctionT*))));
	for(IntT i = 0; i < nnStruct->nHFTuples; i++){
		FAILIF(NULL == (lshFunctions[i] = (LSHFunctionT*)MALLOC(nnStruct->hfTuplesLength * sizeof(LSHFunctionT))));
		for(IntT j = 0; j < nnStruct->hfTuplesLength; j++){
			FAILIF(NULL == (lshFunctions[i][j].a = (RealT*)MALLOC(nnStruct->dimension * sizeof(RealT))));
		}
	}

	// initialize the LSH functions
	for(IntT i = 0; i < nnStruct->nHFTuples; i++){
		for(IntT j = 0; j < nnStruct->hfTuplesLength; j++){
			// vector a
			for(IntT d = 0; d < nnStruct->dimension; d++){
#ifdef USE_L1_DISTANCE
				lshFunctions[i][j].a[d] = getCauchyRandom();
#else
				fread(&lshFunctions[i][j].a[d], sizeof(lshFunctions[i][j].a[d]), 1, indexFile);
#endif
			}
			// b
			fread(&lshFunctions[i][j].b, sizeof(lshFunctions[i][j].b), 1, indexFile);
		}
	}

	FAILIF(NULL == (nnStruct->hashedBuckets = (PUHashStructureT*)MALLOC(nnStruct->parameterL * sizeof(PUHashStructureT))));
	for (int i = 0; i < nnStruct->parameterL; i++) {
		FAILIF(NULL == (nnStruct->hashedBuckets[i] = (UHashStructureT *)MALLOC(nnStruct->parameterL * sizeof(UHashStructureT))));
		nnStruct->hashedBuckets[i]->hashedDataLength = nnStruct->parameterK;
		FAILIF(NULL == (nnStruct->hashedBuckets[i]->mainHashA = (unsigned int*)MALLOC(nnStruct->parameterK * sizeof(unsigned int))));
		FAILIF(NULL == (nnStruct->hashedBuckets[i]->controlHash1 = (unsigned int*)MALLOC(nnStruct->parameterK * sizeof(unsigned int))));
		for (int j = 0; j < nnStruct->parameterK; j++) {

			fread(&nnStruct->hashedBuckets[i]->mainHashA[j], sizeof(nnStruct->hashedBuckets[i]->mainHashA[j]), 1, indexFile);
			fread(&nnStruct->hashedBuckets[i]->controlHash1[j], sizeof(nnStruct->hashedBuckets[i]->controlHash1[j]), 1, indexFile);
		}
	}

	nnStruct->lshFunctions = lshFunctions;
}

/*string getDataSetFileNameFromIndex(char* indexFile) {
	//rewind(indexFile);

	memcpy(sBuffer, indexFile, sizeof(char) * MAX_FILE_NAME_LENGTH);
	//fread(sBuffer, sizeof(char), 

	return string(sBuffer);
}

#define WIN32_LEAN_AND_MEAN 
#include <windows.h> 

typedef struct 
{ 
   HANDLE f; 
   HANDLE m; 
   void *p; 
} SIMPLE_UNMMAP; 

// map 'filename' and return a pointer to it. fill out *length and *un if not-NULL 
void *simple_mmap(const char *filename, int *length, SIMPLE_UNMMAP *un) 
{ 
	HANDLE f = CreateFileA(filename, GENERIC_READ, FILE_SHARE_READ,  NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL); 
	DWORD error = GetLastError();
	HANDLE m; 
	void *p; 
	if (!f) return NULL; 
	m = CreateFileMapping(f, NULL, PAGE_READONLY, 0,0, NULL); 
	if (!m) { CloseHandle(f); return NULL; } 
	p = MapViewOfFile(m, FILE_MAP_READ, 0,0,0); 
	if (!p) { CloseHandle(m); CloseHandle(f); return NULL; } 
	if (length) *length = GetFileSize(f, NULL); 
	if (un) { 
		un->f = f; 
		un->m = m; 
		un->p = p; 
	} 
	return p; 
} 

void simple_unmmap(SIMPLE_UNMMAP *un) 
{ 
   UnmapViewOfFile(un->p); 
   CloseHandle(un->m); 
   CloseHandle(un->f); 
} 

void queryInner(string queryFileName, string indexFileName, RNNParametersT optParameters, string outputFileName) {
	FILE* queryFile = fopen(queryFileName.c_str(), "r");
	//FILE* indexFile = fopen(indexFileName.c_str(), "rb");
	SIMPLE_UNMMAP mapStructure1, mapStructure2;
	char* indexFile = (char*) simple_mmap(indexFileName.c_str(), NULL, &mapStructure1);
	DWORD error = GetLastError();
	char* datasetFile = (char*) simple_mmap(getDataSetFileNameFromIndex(indexFile).c_str(), NULL, &mapStructure2);
	FILE* outputFile = fopen(outputFileName.c_str(), "w+t");

	//IntT resultSize = nQueries;
	//PPointT *result = (PPointT*)MALLOC(resultSize * sizeof(*result));
	PPointT queryPoint;
	FAILIF(NULL == queryFile);
	FAILIF(NULL == indexFile);
	FAILIF(NULL == datasetFile);
	FAILIF(NULL == outputFile);
	FAILIF(NULL == (queryPoint = (PPointT)MALLOC(sizeof(PointT))));
	FAILIF(NULL == (queryPoint->coordinates = (RealT*)MALLOC(pointsDimension * sizeof(RealT))));

	PRNearNeighborStructT nnStruct = getHashedStructure(optParameters, false, true, indexFile);

	Uns32T *mainHashA = NULL, *controlHash1 = NULL;
	//Uns32T *mainhashABackup = nnStruct->hashedBuckets[0]->mainHashA;
	PUHashStructureT modelHT = newUHashStructure(HT_LINKED_LIST, HASH_TABLE_SIZE, nnStruct->parameterK, TRUE, mainHashA, controlHash1, NULL);

	if (mainHashA == NULL) {
		mainHashA = nnStruct->hashedBuckets[0]->mainHashA;
		modelHT->mainHashA = nnStruct->hashedBuckets[0]->mainHashA;
	}

	if (controlHash1 == NULL) {
		controlHash1 = nnStruct->hashedBuckets[0]->controlHash1;
		modelHT->controlHash1 = nnStruct->hashedBuckets[0]->controlHash1;
	}
	// Uns32T **(precomputedHashesOfULSHs[nnStruct->nHFTuples]);
	Uns32T *** precomputedHashesOfULSHs = (Uns32T ***) alloca(nnStruct->nHFTuples * sizeof(Uns32T**));
	for(IntT l = 0; l < nnStruct->nHFTuples; l++){
		FAILIF(NULL == (precomputedHashesOfULSHs[l] = (Uns32T**)MALLOC(sizeof(Uns32T*))));
		for(IntT i = 0; i < 1; i++){
			FAILIF(NULL == (precomputedHashesOfULSHs[l][i] = (Uns32T*)MALLOC(N_PRECOMPUTED_HASHES_NEEDED * sizeof(Uns32T))));
		}
	}

	map<string, int> picCount;
	map<int, int> nnPoints;

	string prevFile = "UNINITIALIZED";
	string currentFile;
	map<pair<int, unsigned int>, int> bucketMatch;

	while (true) {
		FAILIF(queryFile == NULL);

		bool isEOF = false;

		if (fscanf(queryFile, "%s", sBuffer) == EOF) {
			isEOF = true;
		}
		currentFile = string(sBuffer);
		currentFile = currentFile.substr(0, currentFile.find_last_of('_'));

		if ((prevFile != "UNINITIALIZED" && currentFile != prevFile) || isEOF) {
			int sBufferSize = 0;
			while (sBuffer[sBufferSize] != '\0') sBufferSize++;
			for (int j = sBufferSize - 1; j >= 0; j--) {
				ungetc(sBuffer[j], queryFile);
			}

			//std::sort(bucketMatch.begin(), bucketMatch.end());

			//rewind(indexFile);
			char* current = indexFile;

			current += sizeof(char) * MAX_FILE_NAME_LENGTH;
			int prevIndex = -1;
			for (map<pair<int, unsigned int>, int>::iterator it = bucketMatch.begin(); it != bucketMatch.end(); it++) {
				int hIndex = it->first.first;
				unsigned int hValue = it->first.second;

				current += ONE_ROW * (hIndex - prevIndex - 1);

				int num;

				// 得到当前的num
				memcpy(&num, current, sizeof(int));
				current += sizeof(int);

				for (int i = 0; i < num; i++) {
					int curIndex;
					unsigned int curValue;
					memcpy(&curIndex, current, sizeof(curIndex));
					current += sizeof(curIndex);
					memcpy(&curValue, current, sizeof(curValue));
					current += sizeof(curValue);

					if (curValue == hValue) {
						nnPoints[curIndex] += it->second;
					}
				}
				
				current += (sizeof(int) + sizeof(unsigned int)) * (MAX_IN_ONE_BUCKET - num);
				prevIndex = hIndex;
			}

			//sort(nnPoints.begin(), nnPoints.end());

			//rewind(datasetFile);
			current = datasetFile;
			prevIndex = -1;

			for (map<int, int>::iterator it = nnPoints.begin(); it != nnPoints.end(); it++) {
				current += DATASET_ONE_ROW_SIZE * (it->first - prevIndex - 1);

				memcpy(sBuffer, current, sizeof(char) * MAX_FILE_NAME_LENGTH);
				current += sizeof(char) * MAX_FILE_NAME_LENGTH;

				string pointsId = string(sBuffer);
				picCount[pointsId.substr(0, pointsId.find_last_of('_'))] += it->second;

				current += DATASET_ONE_ROW_SIZE - MAX_FILE_NAME_LENGTH * sizeof(char);
				prevIndex = it->first;
			}

			vector<pair<int, string> > matchedPicture;
			for (map<string, int>::iterator pos = picCount.begin(); pos != picCount.end(); pos++) {
				if (pos->second >= MIN_MATCH_GAP) {
					matchedPicture.push_back(make_pair(pos->second, pos->first));
				}
			}

			sort(matchedPicture.begin(), matchedPicture.end());

			//printf("*************************************************************\n");
			//printf("***************************Result****************************\n");
			//printf("*************************************************************\n");
			for (int i = (int) matchedPicture.size() - 1; i >= 0; i--) {
				//std::cout << matchedPicture[i].second << " " <<  matchedPicture[i].first << std::endl;
				fprintf(outputFile, "%s*%d*", matchedPicture[i].second.c_str(), matchedPicture[i].first);
				//printf("%s(%d) ", matchedPicture[i].second.c_str(), matchedPicture[i].first);
			}
			nnPoints.clear();
			picCount.clear();

			fprintf(outputFile, "\n");
			//printf("\n");

			if (isEOF) break;

			bucketMatch.clear();
		}
		prevFile = currentFile;
		RealT sqrLength = 0;
		// read in the query point.
		int ignore = IGNORE_DIMENSION;
		for(IntT d = 0; d < pointsDimension; d++){
			fscanf(queryFile, "%lf", &(queryPoint->coordinates[d]));
			//FAILIF(fscanf(queryFile, "%lf", &(queryPoint->coordinates[d])) != 1);
			sqrLength += SQR(queryPoint->coordinates[d]);
			if (ignore > 0) ignore--, d--;
		}
		queryPoint->sqrLength = sqrLength;
		//printRealVector("Query: ", pointsDimension, queryPoint->coordinates);
		preparePointAdding(nnStruct, modelHT, queryPoint);
		for(IntT l = 0; l < nnStruct->nHFTuples; l++){
#ifdef MYDEBUG
			if (l < 10) printf("%d: ", l);
#endif
			for(IntT h = 0; h < N_PRECOMPUTED_HASHES_NEEDED; h++){
				precomputedHashesOfULSHs[l][0][h] = nnStruct->precomputedHashesOfULSHs[l][h];
#ifdef MYDEBUG
				if (l < 10) printf(" %d", precomputedHashesOfULSHs[l][i][h]);
#endif
			}
#ifdef MYDEBUG
			if (l < 10) printf("\n");
#endif
		}


		for(IntT j = 0; j < nnStruct->parameterL; j++){
			// build the model HT.
			// Add point <dataSet[p]> to modelHT.
			if (!nnStruct->useUfunctions) {
				// Use usual <g> functions (truly independent; <g>s are precisly
				// <u>s).
				Uns32T hValue = combinePrecomputedHashes(precomputedHashesOfULSHs[j][0], NULL, 1, UHF_MAIN_INDEX);
				int hIndex = hValue % HASH_TABLE_SIZE;

				int* bucket = NULL;

				bucketMatch[std::make_pair(hIndex, hValue)]++;

#ifdef MYDEBUG
				printf(" %d", hIndex);
#endif
			}
		}

		if (isEOF) break;
	}
	//fclose(datasetFile);
	fclose(queryFile);
	//fclose(indexFile);
	fclose(outputFile);
	simple_unmmap(&mapStructure1);
	simple_unmmap(&mapStructure2);

}*/

string getDataSetFileNameFromIndex(FILE* indexFile) {
	rewind(indexFile);

	fread(sBuffer, sizeof(char), MAX_FILE_NAME_LENGTH, indexFile);

	return string(sBuffer);
}
void queryInner(string queryFileName, string indexFileName, RNNParametersT optParameters, string outputFileName) {
	FILE* queryFile = fopen(queryFileName.c_str(), "r");
	FILE* indexFile = fopen(indexFileName.c_str(), "rb");
	FILE* datasetFile = fopen(getDataSetFileNameFromIndex(indexFile).c_str(), "rb");
	FILE* outputFile = fopen(outputFileName.c_str(), "w+t");

	//IntT resultSize = nQueries;
	//PPointT *result = (PPointT*)MALLOC(resultSize * sizeof(*result));
	PPointT queryPoint;
	FAILIF(NULL == queryFile);
	FAILIF(NULL == indexFile);
	FAILIF(NULL == datasetFile);
	FAILIF(NULL == outputFile);
	FAILIF(NULL == (queryPoint = (PPointT)MALLOC(sizeof(PointT))));
	FAILIF(NULL == (queryPoint->coordinates = (RealT*)MALLOC(pointsDimension * sizeof(RealT))));

	PRNearNeighborStructT nnStruct = getHashedStructure(optParameters, false, true, indexFile);

	Uns32T *mainHashA = NULL, *controlHash1 = NULL;
	//Uns32T *mainhashABackup = nnStruct->hashedBuckets[0]->mainHashA;
	PUHashStructureT modelHT = newUHashStructure(HT_LINKED_LIST, HASH_TABLE_SIZE, nnStruct->parameterK, TRUE, mainHashA, controlHash1, NULL);

	if (mainHashA == NULL) {
		mainHashA = nnStruct->hashedBuckets[0]->mainHashA;
		modelHT->mainHashA = nnStruct->hashedBuckets[0]->mainHashA;
	}

	if (controlHash1 == NULL) {
		controlHash1 = nnStruct->hashedBuckets[0]->controlHash1;
		modelHT->controlHash1 = nnStruct->hashedBuckets[0]->controlHash1;
	}
	// Uns32T **(precomputedHashesOfULSHs[nnStruct->nHFTuples]);
	Uns32T *** precomputedHashesOfULSHs = (Uns32T ***) alloca(nnStruct->nHFTuples * sizeof(Uns32T**));
	for(IntT l = 0; l < nnStruct->nHFTuples; l++){
		FAILIF(NULL == (precomputedHashesOfULSHs[l] = (Uns32T**)MALLOC(sizeof(Uns32T*))));
		for(IntT i = 0; i < 1; i++){
			FAILIF(NULL == (precomputedHashesOfULSHs[l][i] = (Uns32T*)MALLOC(N_PRECOMPUTED_HASHES_NEEDED * sizeof(Uns32T))));
		}
	}

	map<string, int> picCount;
	map<int, int> nnPoints;

	string prevFile = "UNINITIALIZED";
	string currentFile;
	map<pair<int, unsigned int>, int> bucketMatch;

	while (true) {
		FAILIF(queryFile == NULL);

		bool isEOF = false;

		if (fscanf(queryFile, "%s", sBuffer) == EOF) {
			isEOF = true;
		}
		currentFile = string(sBuffer);
		currentFile = currentFile.substr(0, currentFile.find_last_of('_'));

		if ((prevFile != "UNINITIALIZED" && currentFile != prevFile) || isEOF) {
			int sBufferSize = 0;
			while (sBuffer[sBufferSize] != '\0') sBufferSize++;
			for (int j = sBufferSize - 1; j >= 0; j--) {
				ungetc(sBuffer[j], queryFile);
			}

			//std::sort(bucketMatch.begin(), bucketMatch.end());

			rewind(indexFile);

			fseek(indexFile, sizeof(char) * MAX_FILE_NAME_LENGTH, SEEK_CUR);
			int prevIndex = -1;
			for (map<pair<int, unsigned int>, int>::iterator it = bucketMatch.begin(); it != bucketMatch.end(); it++) {
				int hIndex = it->first.first;
				unsigned int hValue = it->first.second;

				fseek(indexFile, ONE_ROW * (hIndex - prevIndex - 1), SEEK_CUR);

				int num;

				// 得到当前的num
				fread(&num, sizeof(int), 1, indexFile);

				for (int i = 0; i < num; i++) {
					int curIndex;
					unsigned int curValue;
					fread(&curIndex, sizeof(curIndex), 1, indexFile);
					fread(&curValue, sizeof(curValue), 1, indexFile);

					if (curValue == hValue) {
						nnPoints[curIndex] += it->second;
					}
				}
				
				fseek(indexFile, (sizeof(int) + sizeof(unsigned int)) * (MAX_IN_ONE_BUCKET - num), SEEK_CUR);
				prevIndex = hIndex;
			}

			//sort(nnPoints.begin(), nnPoints.end());

			rewind(datasetFile);
			prevIndex = -1;

			for (map<int, int>::iterator it = nnPoints.begin(); it != nnPoints.end(); it++) {
				fseek(datasetFile, DATASET_ONE_ROW_SIZE * (it->first - prevIndex - 1), SEEK_CUR);
				fread(sBuffer,sizeof(char), MAX_FILE_NAME_LENGTH, datasetFile);

				string pointsId = string(sBuffer);
				picCount[pointsId.substr(0, pointsId.find_last_of('_'))] += it->second;

				fseek(datasetFile, DATASET_ONE_ROW_SIZE - MAX_FILE_NAME_LENGTH * sizeof(char), SEEK_CUR);
				prevIndex = it->first;
			}

			vector<pair<int, string> > matchedPicture;
			for (map<string, int>::iterator pos = picCount.begin(); pos != picCount.end(); pos++) {
				if (pos->second >= MIN_MATCH_GAP) {
					matchedPicture.push_back(make_pair(pos->second, pos->first));
				}
			}

			sort(matchedPicture.begin(), matchedPicture.end());

			//printf("*************************************************************\n");
			//printf("***************************Result****************************\n");
			//printf("*************************************************************\n");
			for (int i = (int) matchedPicture.size() - 1; i >= 0; i--) {
				//std::cout << matchedPicture[i].second << " " <<  matchedPicture[i].first << std::endl;
				fprintf(outputFile, "%s*%d*", matchedPicture[i].second.c_str(), matchedPicture[i].first);
				//printf("%s(%d) ", matchedPicture[i].second.c_str(), matchedPicture[i].first);
			}
			nnPoints.clear();
			picCount.clear();

			fprintf(outputFile, "\n");
			//printf("\n");

			if (isEOF) break;

			bucketMatch.clear();
		}
		prevFile = currentFile;
		RealT sqrLength = 0;
		// read in the query point.
		int ignore = IGNORE_DIMENSION;
		for(IntT d = 0; d < pointsDimension; d++){
			fscanf(queryFile, "%lf", &(queryPoint->coordinates[d]));
			//FAILIF(fscanf(queryFile, "%lf", &(queryPoint->coordinates[d])) != 1);
			sqrLength += SQR(queryPoint->coordinates[d]);
			if (ignore > 0) ignore--, d--;
		}
		queryPoint->sqrLength = sqrLength;
		//printRealVector("Query: ", pointsDimension, queryPoint->coordinates);
		preparePointAdding(nnStruct, modelHT, queryPoint);
		for(IntT l = 0; l < nnStruct->nHFTuples; l++){
#ifdef MYDEBUG
			if (l < 10) printf("%d: ", l);
#endif
			for(IntT h = 0; h < N_PRECOMPUTED_HASHES_NEEDED; h++){
				precomputedHashesOfULSHs[l][0][h] = nnStruct->precomputedHashesOfULSHs[l][h];
#ifdef MYDEBUG
				if (l < 10) printf(" %d", precomputedHashesOfULSHs[l][i][h]);
#endif
			}
#ifdef MYDEBUG
			if (l < 10) printf("\n");
#endif
		}


		for(IntT j = 0; j < nnStruct->parameterL; j++){
			// build the model HT.
			// Add point <dataSet[p]> to modelHT.
			if (!nnStruct->useUfunctions) {
				// Use usual <g> functions (truly independent; <g>s are precisly
				// <u>s).
				Uns32T hValue = combinePrecomputedHashes(precomputedHashesOfULSHs[j][0], NULL, 1, UHF_MAIN_INDEX);
				int hIndex = hValue % HASH_TABLE_SIZE;

				int bucketSize;
				int* bucket = NULL;

				bucketMatch[std::make_pair(hIndex, hValue)]++;

#ifdef MYDEBUG
				printf(" %d", hIndex);
#endif
			}
		}

		if (isEOF) break;
	}
	fclose(datasetFile);
	fclose(queryFile);
	fclose(indexFile);
	fclose(outputFile);
}

double computeAlpha(string indexFileName) {
	FILE *fp = fopen(indexFileName.c_str(), "rb");

	int cnt = 0;
	fread(sBuffer, sizeof(char), MAX_FILE_NAME_LENGTH, fp);

	for (int i = 0; i < BUCKET_NUM; i++) {
		int num;
		fread(&num, sizeof(num), 1, fp);

		fseek(fp, (ONE_ROW - sizeof(int)), SEEK_CUR);

		cnt += num;
	}

	fclose(fp);

	return (double) cnt / ((double) BUCKET_NUM * MAX_IN_ONE_BUCKET);
}

string getPictureId(FILE* datasetFile, int index) {
	//map<int, string>::iterator pos = (*index2name).find(index);

	//if (pos != index2name->end()) {
	//	return pos->second;
	//}

	rewind(datasetFile);
    fseek(datasetFile, DATASET_ONE_ROW_SIZE * index, SEEK_SET);
	fread(sBuffer,sizeof(char), MAX_FILE_NAME_LENGTH, datasetFile);

	string pointsId = string(sBuffer);
	return pointsId.substr(0, pointsId.find_last_of('_'));
}
PPointT getPointFromDataset(FILE* datasetFile, int index) {
	rewind(datasetFile);
	PPointT p;
	RealT sqrLength = 0;
	FAILIF(NULL == (p = (PPointT)MALLOC(sizeof(PointT))));
	FAILIF(NULL == (p->coordinates = (RealT*)MALLOC(pointsDimension * sizeof(RealT))));

	fseek(datasetFile, DATASET_ONE_ROW_SIZE * index + sizeof(char) * MAX_FILE_NAME_LENGTH, SEEK_SET);

	for(IntT d = 0; d < pointsDimension; d++){

		//if (fscanf(datasetFile, "%lf", &(p->coordinates[d])) == EOF) {
		if (fread(&(p->coordinates[d]), sizeof(double), 1, datasetFile) != 1) {
			break;
		}

		sqrLength += SQR(p->coordinates[d]);
	}

	p->index = -1;
	p->sqrLength = sqrLength;

	return p;
}
int* getBucketFromDisk(FILE* fp, int hIndex, Uns32T hValue, int& bucketSize) {
	FAILIF(fp == NULL);
	rewind(fp);

	fseek(fp, sizeof(char) * MAX_FILE_NAME_LENGTH + ONE_ROW * hIndex, SEEK_SET);

	int num;

	// 得到当前的num
	fread(&num, sizeof(int), 1, fp);

	bucketSize = num;

	if (num == 0) return NULL;

	int* ret = (int*) malloc(sizeof(int) * num);

	int cnt = 0;
	Uns32T theValue;
	for (int i = 0; i < num; i++) {
		fread(&ret[cnt], sizeof(ret[cnt]), 1, fp);
		fread(&theValue, sizeof(theValue), 1, fp);

		if (theValue == hValue) {
			cnt++;
		}
	}

	bucketSize = cnt;
	return ret;
}

void addOnePointToBucket(FILE *fp, int bucketIndex, int pointIndex, Uns32T hValue)
{
	FAILIF(fp == NULL);
	rewind(fp);

	fseek(fp,  sizeof(char) * MAX_FILE_NAME_LENGTH + ONE_ROW * bucketIndex, SEEK_SET);

	int num;

	// 得到当前的num计数然后写入新的数据
	fread(&num, sizeof(int), 1, fp);

	if (num < MAX_IN_ONE_BUCKET) {
		fseek(fp, num * ONE_ELEMENT, SEEK_CUR);

		fwrite(&pointIndex, sizeof(int), 1, fp);
		fwrite(&hValue, sizeof(Uns32T), 1, fp);

		num++;

		fseek(fp, sizeof(char) * MAX_FILE_NAME_LENGTH + ONE_ROW * bucketIndex, SEEK_SET);

		fwrite(&num, sizeof(int), 1, fp);
	}

	rewind(fp);
}

/// 初始化索引文件，索引文件首先会包含长度为MAX_FILE_NAME_LENGTH的数据文件名，然后是所有bucket.
void initAllDiskBucket(FILE* fp, string dataSetFileName)
{
	FAILIF(fp == NULL);

	rewind(fp);

	fwrite(dataSetFileName.c_str(), sizeof(char), MAX_FILE_NAME_LENGTH, fp);

	for (int i = 0; i < BUCKET_NUM; i++) {
		int cnt = 0;
		Uns32T value = 0;
		FAILIF(1 != fwrite(&cnt, sizeof(int), 1, fp));
		//printf("%d\n", fwrite(&cnt, sizeof(int), 1, fp));
		for (int j = 0; j < MAX_IN_ONE_BUCKET; j++) {
			fwrite(&cnt, sizeof(cnt), 1, fp);
			fwrite(&value, sizeof(value), 1, fp);
		}
	}
}

PRNearNeighborStructT getHashedStructure(RNNParametersT algParameters, bool isFirst, bool initFromFile, FILE* indexFile){
	PRNearNeighborStructT nnStruct;
	FAILIF(NULL == (nnStruct = (PRNearNeighborStructT)MALLOC(sizeof(RNearNeighborStructT))));
	nnStruct->parameterR = algParameters.parameterR;
	nnStruct->parameterR2 = algParameters.parameterR2;
	nnStruct->useUfunctions = algParameters.useUfunctions;
	nnStruct->parameterK = algParameters.parameterK;
	if (!algParameters.useUfunctions) {
		// Use normal <g> functions.
		nnStruct->parameterL = algParameters.parameterL;
		nnStruct->nHFTuples = algParameters.parameterL;
		nnStruct->hfTuplesLength = algParameters.parameterK;
	}else{
		// Use <u> hash functions; a <g> function is a pair of 2 <u> functions.
		nnStruct->parameterL = algParameters.parameterL;
		nnStruct->nHFTuples = algParameters.parameterM;
		nnStruct->hfTuplesLength = algParameters.parameterK / 2;
	}
	nnStruct->parameterT = algParameters.parameterT;
	nnStruct->dimension = algParameters.dimension;
	nnStruct->parameterW = algParameters.parameterW;

	nnStruct->nPoints = 0;
	nnStruct->pointsArraySize = MAX_POINTS_IN_ONE_PASS;

	FAILIF(NULL == (nnStruct->points = (PPointT*)MALLOC(nnStruct->pointsArraySize * sizeof(PPointT))));

	//FAILIF(NULL == (nnStruct->hashedBuckets = (PUHashStructureT*)MALLOC(nnStruct->parameterL * sizeof(PUHashStructureT))));

	// create the hash functions
	// 如果是第一次进入，初始化hash函数，否则仍然使用之前的hash函数
	if (isFirst) {
		initHashFunctions(nnStruct);
	} else if (initFromFile) {
		getHashfunction(nnStruct, indexFile);
	}

	// init fields that are used only in operations ("temporary" variables for operations).

	// init the vector <pointULSHVectors> and the vector
	// <precomputedHashesOfULSHs>
	FAILIF(NULL == (nnStruct->pointULSHVectors = (Uns32T**)MALLOC(nnStruct->nHFTuples * sizeof(Uns32T*))));
	for(IntT i = 0; i < nnStruct->nHFTuples; i++){
		FAILIF(NULL == (nnStruct->pointULSHVectors[i] = (Uns32T*)MALLOC(nnStruct->hfTuplesLength * sizeof(Uns32T))));
	}
	FAILIF(NULL == (nnStruct->precomputedHashesOfULSHs = (Uns32T**)MALLOC(nnStruct->nHFTuples * sizeof(Uns32T*))));
	for(IntT i = 0; i < nnStruct->nHFTuples; i++){
		FAILIF(NULL == (nnStruct->precomputedHashesOfULSHs[i] = (Uns32T*)MALLOC(N_PRECOMPUTED_HASHES_NEEDED * sizeof(Uns32T))));
	}
	// init the vector <reducedPoint>
	FAILIF(NULL == (nnStruct->reducedPoint = (RealT*)MALLOC(nnStruct->dimension * sizeof(RealT))));
	// init the vector <nearPoints>
	nnStruct->sizeMarkedPoints = MAX_POINTS_IN_ONE_PASS;
	FAILIF(NULL == (nnStruct->markedPoints = (BooleanT*)MALLOC(nnStruct->sizeMarkedPoints * sizeof(BooleanT))));
	for(IntT i = 0; i < nnStruct->sizeMarkedPoints; i++){
		nnStruct->markedPoints[i] = FALSE;
	}
	// init the vector <nearPointsIndeces>
	FAILIF(NULL == (nnStruct->markedPointsIndeces = (Int32T*)MALLOC(nnStruct->sizeMarkedPoints * sizeof(Int32T))));

	nnStruct->reportingResult = TRUE;

	return nnStruct;
}

PRNearNeighborStructT getHashedStructure(RNNParametersT algParameters, bool isFirst, bool initFromFile, char* indexFile){
	PRNearNeighborStructT nnStruct;
	FAILIF(NULL == (nnStruct = (PRNearNeighborStructT)MALLOC(sizeof(RNearNeighborStructT))));
	nnStruct->parameterR = algParameters.parameterR;
	nnStruct->parameterR2 = algParameters.parameterR2;
	nnStruct->useUfunctions = algParameters.useUfunctions;
	nnStruct->parameterK = algParameters.parameterK;
	if (!algParameters.useUfunctions) {
		// Use normal <g> functions.
		nnStruct->parameterL = algParameters.parameterL;
		nnStruct->nHFTuples = algParameters.parameterL;
		nnStruct->hfTuplesLength = algParameters.parameterK;
	}else{
		// Use <u> hash functions; a <g> function is a pair of 2 <u> functions.
		nnStruct->parameterL = algParameters.parameterL;
		nnStruct->nHFTuples = algParameters.parameterM;
		nnStruct->hfTuplesLength = algParameters.parameterK / 2;
	}
	nnStruct->parameterT = algParameters.parameterT;
	nnStruct->dimension = algParameters.dimension;
	nnStruct->parameterW = algParameters.parameterW;

	nnStruct->nPoints = 0;
	nnStruct->pointsArraySize = MAX_POINTS_IN_ONE_PASS;

	FAILIF(NULL == (nnStruct->points = (PPointT*)MALLOC(nnStruct->pointsArraySize * sizeof(PPointT))));

	//FAILIF(NULL == (nnStruct->hashedBuckets = (PUHashStructureT*)MALLOC(nnStruct->parameterL * sizeof(PUHashStructureT))));

	// create the hash functions
	// 如果是第一次进入，初始化hash函数，否则仍然使用之前的hash函数
	if (isFirst) {
		initHashFunctions(nnStruct);
	} else if (initFromFile) {
		getHashfunction(nnStruct, indexFile);
	}

	// init fields that are used only in operations ("temporary" variables for operations).

	// init the vector <pointULSHVectors> and the vector
	// <precomputedHashesOfULSHs>
	FAILIF(NULL == (nnStruct->pointULSHVectors = (Uns32T**)MALLOC(nnStruct->nHFTuples * sizeof(Uns32T*))));
	for(IntT i = 0; i < nnStruct->nHFTuples; i++){
		FAILIF(NULL == (nnStruct->pointULSHVectors[i] = (Uns32T*)MALLOC(nnStruct->hfTuplesLength * sizeof(Uns32T))));
	}
	FAILIF(NULL == (nnStruct->precomputedHashesOfULSHs = (Uns32T**)MALLOC(nnStruct->nHFTuples * sizeof(Uns32T*))));
	for(IntT i = 0; i < nnStruct->nHFTuples; i++){
		FAILIF(NULL == (nnStruct->precomputedHashesOfULSHs[i] = (Uns32T*)MALLOC(N_PRECOMPUTED_HASHES_NEEDED * sizeof(Uns32T))));
	}
	// init the vector <reducedPoint>
	FAILIF(NULL == (nnStruct->reducedPoint = (RealT*)MALLOC(nnStruct->dimension * sizeof(RealT))));
	// init the vector <nearPoints>
	nnStruct->sizeMarkedPoints = MAX_POINTS_IN_ONE_PASS;
	FAILIF(NULL == (nnStruct->markedPoints = (BooleanT*)MALLOC(nnStruct->sizeMarkedPoints * sizeof(BooleanT))));
	for(IntT i = 0; i < nnStruct->sizeMarkedPoints; i++){
		nnStruct->markedPoints[i] = FALSE;
	}
	// init the vector <nearPointsIndeces>
	FAILIF(NULL == (nnStruct->markedPointsIndeces = (Int32T*)MALLOC(nnStruct->sizeMarkedPoints * sizeof(Int32T))));

	nnStruct->reportingResult = TRUE;

	return nnStruct;
}


// Creates the LSH hash functions for the R-near neighbor structure
// <nnStruct>. The functions fills in the corresponding field of
// <nnStruct>.
void initHashF(PRNearNeighborStructT nnStruct){
	ASSERT(nnStruct != NULL);
	LSHFunctionT **lshFunctions;
	// allocate memory for the functions
	FAILIF(NULL == (lshFunctions = (LSHFunctionT**)MALLOC(nnStruct->nHFTuples * sizeof(LSHFunctionT*))));
	for(IntT i = 0; i < nnStruct->nHFTuples; i++){
		FAILIF(NULL == (lshFunctions[i] = (LSHFunctionT*)MALLOC(nnStruct->hfTuplesLength * sizeof(LSHFunctionT))));
		for(IntT j = 0; j < nnStruct->hfTuplesLength; j++){
			FAILIF(NULL == (lshFunctions[i][j].a = (RealT*)MALLOC(nnStruct->dimension * sizeof(RealT))));
		}
	}

	// initialize the LSH functions
	for(IntT i = 0; i < nnStruct->nHFTuples; i++){
		for(IntT j = 0; j < nnStruct->hfTuplesLength; j++){
			// vector a
			for(IntT d = 0; d < nnStruct->dimension; d++){
#ifdef USE_L1_DISTANCE
				lshFunctions[i][j].a[d] = getCauchyRandom();
#else
				lshFunctions[i][j].a[d] = genGaussianRandom();
#endif
			}
			// b
			lshFunctions[i][j].b = genUniformRandom(0, nnStruct->parameterW);
		}
	}

	nnStruct->lshFunctions = lshFunctions;
}
