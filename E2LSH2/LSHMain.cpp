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

int parameterL = 2;

// The succes probability of each point (each near neighbor is
// reported by the algorithm with probability <successProbability>).
RealT successProbability = 0.9;

char sBuffer[512];


// 以下内容为项目中添加的内容

// 用到的一些常量
// 保存到文件
const int IGNORE_DIMENSION = 4;
const int MAX_IN_ONE_BUCKET = 200;
const int MAX_POINTS_IN_ONE_PASS = 10000;
const int MAX_FILE_NAME_LENGTH = 256;
const int DATASET_FILE_INFO_SIZE = MAX_FILE_NAME_LENGTH * sizeof(char) + sizeof(Long64T) + sizeof(int);
const int MIN_BUCKET_NUM = 50000;
const int MIN_MATCH_GAP = 5;
const int ONE_ELEMENT = sizeof(int) + sizeof(Uns32T);
const int ONE_ROW = (sizeof(int) + sizeof(Uns32T)) * MAX_IN_ONE_BUCKET + sizeof(int);
const int DATASET_ONE_ROW_SIZE = sizeof(Long64T) + sizeof(int) + sizeof(double) * (pointsDimension + IGNORE_DIMENSION);
// default value, can modified by a parameter.
int bucketNum = MIN_BUCKET_NUM;
int hashTableSize = bucketNum;
int pointNumLimit = 0;	// point number limit of a data file
typedef struct _pointId
{
	Long64T id;
	int index;
} PointId;
PointId pointId;

//#define MEMORY_DEBUG // for print debug information

#define PRINT_MEMORY(pre) {printf("%s used memory = %d\n", pre, totalAllocatedMemory);}

void setUpIndexFromDataSet(string dataSetFileName, string indexFileName, RNNParametersT optParameters, bool isAdd=false, int minBucketNum=MIN_BUCKET_NUM, Long64T existNPoints=0);
void addOnePointToBucket(FILE *fp, int bucketIndex, int pointIndex,Uns32T hValue);
void initAllDiskBucket(FILE* fp, string dataFileName, Long64T allPointsNum, int pointNumLimit);
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
string getFileName(const string& filePath); // c:\a\b.c => b.c
string getWholeFilePath(const string& filePath, const string& fileName); // c:\a\b.c, d.e => c:\a\d.e
void addDataSetToIndexDataSet(const string& dataFileName, const string& indexFileName);
void printHashfunction(PRNearNeighborStructT nnStruct, string name);
void printUHashStructureT(PUHashStructureT hashS, string name);

inline void printMemory(char* pre)
{
#ifdef MEMORY_DEBUG
	PRINT_MEMORY(pre);
#endif
}

void saveParameter(string indexName, double inputR, double inputW, int inputK, int inputL, int theBucketNum) {
	FILE* indexFile = fopen(indexName.c_str(), "a+b");
	
	fseek(indexFile, 0, SEEK_END);
	fwrite(&inputR, sizeof(inputR), 1, indexFile);
	fwrite(&inputW, sizeof(inputW), 1, indexFile);
	fwrite(&inputK, sizeof(inputK), 1, indexFile);
	fwrite(&inputL, sizeof(inputL), 1, indexFile);
	fwrite(&theBucketNum, sizeof(theBucketNum), 1, indexFile);
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
	saveParameter(indexName, inputR, inputW, inputK, inputL, bucketNum);
}

void getParameter(string indexName, double& inputR, double& inputW, int& inputK, int& inputL, int& theBucketNum) {
	FILE* indexFile = fopen(indexName.c_str(), "rb");

	fseek(indexFile, -((int) sizeof(inputR) + (int) sizeof(inputW) + (int) sizeof(inputK) + (int) sizeof(inputL) + (int)sizeof(theBucketNum)), SEEK_END);
	fread(&inputR, sizeof(inputR), 1, indexFile);
	fread(&inputW, sizeof(inputW), 1, indexFile);
	fread(&inputK, sizeof(inputK), 1, indexFile);
	fread(&inputL, sizeof(inputL), 1, indexFile);
	fread(&theBucketNum, sizeof(theBucketNum), 1, indexFile);
	fclose(indexFile);
}

// add dataset to the index.
#ifdef WIN32
extern "C" __declspec(dllexport) void addToIndex(char* dataFileStr, char* indexNameStr) {
#else
extern "C" void addToIndex(char* dataFileStr, char* indexNameStr) {
#endif // WIN32

	string dataFileName(dataFileStr);
	string indexName(indexNameStr);

	/*copy from query*/
	double inputR, inputW;
	int inputK, inputL;
	int theBucketNum;
	getParameter(indexName, inputR, inputW, inputK, inputL, theBucketNum);
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
	/*********************/
	
	FILE* indexFile = fopen(indexName.c_str(), "rb");
	FAILIF(indexFile == NULL);
	FILE* dataFile = fopen(dataFileName.c_str(), "rb");
	FAILIF(dataFile == NULL);

	Long64T indexAllPointsNum = 0;
	fseek(indexFile, MAX_FILE_NAME_LENGTH * sizeof(char), SEEK_SET);
	FAILIF(1 != fread(&indexAllPointsNum, sizeof(Long64T), 1, indexFile));
	
	Long64T dataAllPointsNum = 0;
	FAILIF(1 != fread(&dataAllPointsNum, sizeof(Long64T), 1, dataFile));
	fclose(dataFile);
	fclose(indexFile);

	// 把数据文件加到index的数据文件中, 并更新index开头的总点数。
	addDataSetToIndexDataSet(dataFileName, indexName);
	

	Long64T newAllPointsNum = indexAllPointsNum + dataAllPointsNum;
	// if 80% full, rehash.
	if (newAllPointsNum * optParameters.parameterL > MAX_IN_ONE_BUCKET * theBucketNum * 0.8)
	{
		setUpIndexFromDataSet(dataFileName, indexName, optParameters, false, theBucketNum * 2);
		saveParameter(indexName, inputR, inputW, inputK, inputL, bucketNum);
	}
	else
	{
		setUpIndexFromDataSet(dataFileName, indexName, optParameters, true, theBucketNum, indexAllPointsNum);
	}

}



#ifdef WIN32
extern "C" __declspec(dllexport) void query(char* queryFileStr, char* indexNameStr, char* outputFileStr) {
#else
extern "C" void query(char* queryFileStr, char* indexNameStr, char* outputFileStr) {
#endif // WIN32
	printMemory("query end");
	string queryFile(queryFileStr);
	string indexName(indexNameStr);
	string outputFile(outputFileStr);

	double inputR, inputW;
	int inputK, inputL;
	int theBucketNum;
	getParameter(indexName, inputR, inputW, inputK, inputL, theBucketNum);
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

	printMemory("query end");
}

void outputIndexFile(string indexFileName, string outputTextFileName) {
	FILE *fp = fopen(indexFileName.c_str(), "rb");
	FILE *output = fopen(outputTextFileName.c_str(), "w+t");

	fread(sBuffer, sizeof(char), MAX_FILE_NAME_LENGTH, fp);
	Long64T allPointsNum;
	int pointNumLimit;
	fread(&allPointsNum, sizeof(Long64T), 1, fp);
	fread(&pointNumLimit, sizeof(int), 1, fp);

	fprintf(output, "%s, %I64d, %d\n", sBuffer, allPointsNum, pointNumLimit);
	int cnt = 0;
	for (int i = 0; i < bucketNum; i++) {
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
// no use
/*
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
*/
// 加入到index的过程, PRNearNeighborStructT->hashedBuckets是不改变的, 所以只要第一次计算出来就可以了. 原来的代码是每个PASS都NEW一次.
// 似乎L(这里是40)个Hash函数是一模一样的, 程序乱写的???
// parameter isAdd   mark whether the dataset is added to exist index, or setup a new one.
// parameter minBucketNum    used when setup a new index, as the minimal bucket number of the new index.
// parameter existNPoints    the number of exist points number when isAdd is true.
//
void setUpIndexFromDataSet(string dataSetFileName, string indexFileName, RNNParametersT optParameters, bool isAdd/*=false*/, int minBucketNum/*=MIN_BUCKET_NUM*/, Long64T existNPoints/*=0*/)
{
    //convertTextDataFile2BinFile(dataSetFileName);
	//bucketNum = 
	//	hashTableSize = bucketNum;

	FILE *datasetFile = fopen(dataSetFileName.c_str(), "rb");
	FAILIF(datasetFile == NULL);
	// the number of dataset files, for the dataset maybe divided into several files if the dataset is too large.
	Long64T allPointsNum = 0;
	fread(&allPointsNum, sizeof(Long64T), 1, datasetFile);
	fread(&pointNumLimit, sizeof(int), 1, datasetFile);
	int nFiles = allPointsNum / pointNumLimit + 1;

	FILE *indexFile = 0;
	PRNearNeighborStructT nnStruct = 0;
	if (isAdd)
	{
		indexFile = fopen(indexFileName.c_str(), "r+b");
		FAILIF(indexFile == NULL);
		nnStruct = getHashedStructure(optParameters, false, true, indexFile);
		bucketNum = minBucketNum;
		hashTableSize = bucketNum;
	}
	else
	{
		// calculate the bucket number and hash table size, at lease half full.
		bucketNum = optParameters.parameterL * (allPointsNum * 2 / MAX_IN_ONE_BUCKET + 1);
		if (bucketNum < minBucketNum)
		{
			bucketNum = minBucketNum;
		}
		hashTableSize = bucketNum;

		indexFile = fopen(indexFileName.c_str(), "w+b");
		FAILIF(indexFile == NULL);

		//初始化文件表
		initAllDiskBucket(indexFile, dataSetFileName, allPointsNum, pointNumLimit);
		nnStruct = getHashedStructure(optParameters, true, false, indexFile);
	}

	bool isFirst = true;
	// 已经加入到index的点数, 用来计算点的下标.
	int cnt = existNPoints;

	Uns32T *mainHashA = NULL, *controlHash1 = NULL;

	double temp[IGNORE_DIMENSION] = {0.0};
	char buf[12] = {'\0'};

	// skip the header of the dataset file
	rewind(datasetFile);
	fseek(datasetFile, sizeof(Long64T) + sizeof(int), SEEK_SET);

	int curFile = 1;
	while (true) {
		// 每次处理一定数目的输入数据点
		FAILIF(NULL == (dataSetPoints = (PPointT*)MALLOC(MAX_POINTS_IN_ONE_PASS * sizeof(PPointT))));

		IntT pointsCurrent = 0;

		for(IntT i = 0; i < MAX_POINTS_IN_ONE_PASS; i++){
			if (fread(&(pointId.id), sizeof(Long64T), 1, datasetFile) != 1)
			{
				if (++curFile > nFiles)
				{
					break;
				}
				fclose(datasetFile);
				sprintf(buf, "%d", curFile);
				string datasetPiece = dataSetFileName;
				datasetFile = fopen(datasetPiece.append(buf).c_str(), "rb");
				FAILIF(datasetFile == NULL);
				if (fread(&(pointId.id), sizeof(Long64T), 1, datasetFile) != 1)
				{
					break;
				}
			}

			PPointT p;
			RealT sqrLength = 0;
			
			FAILIF(NULL == (p = (PPointT)MALLOC(sizeof(PointT))));
			FAILIF(NULL == (p->coordinates = (RealT*)MALLOC(pointsDimension * sizeof(RealT))));
			
			//bool isEnd = false;
			//fread(sBuffer, sizeof(char), MAX_FILE_NAME_LENGTH, datasetFile);
			//fscanf(datasetFile, "%s", sBuffer);

			fread(&(pointId.index), sizeof(int), 1, datasetFile);

			IntT d = 0;
			fread(temp, sizeof(double), IGNORE_DIMENSION, datasetFile);
			for (d = 0; d < IGNORE_DIMENSION; d++)
			{
				sqrLength += SQR(temp[d]);
			}
			fread(p->coordinates, sizeof(double), pointsDimension, datasetFile);
			for (d = 0; d < pointsDimension; d++)
			{
				sqrLength += SQR(p->coordinates[d]);
			}

			p->index = -1;
			p->sqrLength = sqrLength;

			dataSetPoints[i] = p;
			dataSetPoints[i]->index = i;
			pointsCurrent++;
		}

		if (pointsCurrent == 0){
			break;
		}

		// 得到LSH参数结构
		optParameters.parameterT = pointsCurrent;
		//PRNearNeighborStructT *nnStructs = NULL;


		ASSERT(optParameters.typeHT == HT_HYBRID_CHAINS);
		ASSERT(dataSetPoints != NULL);
		ASSERT(USE_SAME_UHASH_FUNCTIONS);

		//printRNNParameters(stderr, optParameters);
		//printf("Started handling points starting from %d.\n", cnt);

		// Set the fields <nPoints> and <points>.
		nnStruct->nPoints = pointsCurrent;
		for(Int32T i = 0; i < pointsCurrent; i++){
			nnStruct->points[i] = dataSetPoints[i];
		}

		// initialize second level hashing (bucket hashing)
		if (isFirst && !isAdd) {
			FAILIF(NULL == (nnStruct->hashedBuckets = (PUHashStructureT*)MALLOC(nnStruct->parameterL * sizeof(PUHashStructureT))));
		}


		// 这里似乎有问题. L个Hash函数都一样的时候这样写没问题.modelHT应该在不同的Hash函数时, 分为nnStruct->hashedBuckets[i].
		// 所以begin1和end1这一段似乎应该放在for(IntT i = 0; i < nnStruct->parameterL; i++){的里面???
		/*********************begin1*************************/
		PUHashStructureT modelHT = NULL;
		if (isFirst) {
			if (isAdd) {
				//Uns32T *mainhashABackup = nnStruct->hashedBuckets[0]->mainHashA;
				modelHT = newUHashStructure(HT_LINKED_LIST, hashTableSize, nnStruct->parameterK, TRUE, mainHashA, controlHash1, NULL);
				mainHashA = nnStruct->hashedBuckets[0]->mainHashA;
				modelHT->mainHashA = mainHashA;
				controlHash1 = nnStruct->hashedBuckets[0]->controlHash1;
				modelHT->controlHash1 = controlHash1;
			} else {
				modelHT = newUHashStructure(HT_LINKED_LIST, hashTableSize, nnStruct->parameterK, FALSE, mainHashA, controlHash1, NULL);
				//printUHashStructureT(modelHT, "E:\\projects\\photodemo\\codes\\PicMatcher\\data\\train\\FirstHashStr");
			}
		} else {
			modelHT = newUHashStructure(HT_LINKED_LIST, hashTableSize, nnStruct->parameterK, TRUE, mainHashA, controlHash1, NULL);
			modelHT->mainHashA = mainHashA;
			modelHT->controlHash1 = controlHash1;
			//printUHashStructureT(modelHT, "E:\\projects\\photodemo\\codes\\PicMatcher\\data\\train\\HashStr");
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
		/*********************end1*************************/

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
//#ifdef MYDEBUG
//			printf("\n");
//#endif
			//ASSERT(nAllocatedGBuckets <= nPoints);
			//ASSERT(nAllocatedBEntries <= nPoints);

			// compute what is the next pair of <u> functions.
			secondUComp++;
			if (secondUComp == nnStruct->nHFTuples) {
				firstUComp++;
				secondUComp = firstUComp + 1;
			}

			if (isFirst && !isAdd) {
				// copy the model HT into the actual (packed) HT. copy the uhash function too.
				nnStruct->hashedBuckets[i] = newUHashStructure(optParameters.typeHT, hashTableSize, nnStruct->parameterK, TRUE, mainHashA, controlHash1, modelHT);
			}

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
		if (isFirst && !isAdd) {
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
		
		/*
		for(IntT i = 0; i < nnStruct->parameterL; i++) {
			FREE(nnStruct->hashedBuckets[i]->hashTable.hybridHashTable);
			FREE(nnStruct->hashedBuckets[i]->hybridChainsStorage);

			FREE(nnStruct->hashedBuckets[i]);
		}
		FREE(nnStruct->hashedBuckets);
		*/
		isFirst = false;

		cnt += pointsCurrent;
	}
	for(IntT i = 0; i < nnStruct->parameterL; i++) {
		FREE(nnStruct->hashedBuckets[i]->hashTable.hybridHashTable);
		FREE(nnStruct->hashedBuckets[i]->hybridChainsStorage);

		FREE(nnStruct->hashedBuckets[i]);
	}
	FREE(nnStruct->hashedBuckets);
	FREE(nnStruct);
	fclose(datasetFile);
	fclose(indexFile);
}

// add the dataset to the exist dataset of the index, it will also modify the indexFile with the new number of all points.
// TODO current just support one dataset file ***************************************
void addDataSetToIndexDataSet(const string& dataFileName, const string& indexFileName)
{
	FILE* indexFile = fopen(indexFileName.c_str(), "r+b");
	FAILIF(indexFile == NULL);
	FILE* dataFile = fopen(dataFileName.c_str(), "rb");
	FAILIF(dataFile == NULL);

	char buf[12] = {'\0'};

	Long64T newAllPointsNum = 0;
	Long64T dataAllPointsNum = 0;
	int indexLimit = 0;
	int dataLimit = 0;

	rewind(indexFile);
	fread(sBuffer, sizeof(char), MAX_FILE_NAME_LENGTH, indexFile);
	fread(&newAllPointsNum, sizeof(Long64T), 1, indexFile);
	fread(&indexLimit, sizeof(int), 1, indexFile);
	int indexNFiles = newAllPointsNum / indexLimit + 1;
	int curPos = newAllPointsNum % indexLimit;

	rewind(dataFile);
	fread(&dataAllPointsNum, sizeof(Long64T), 1, dataFile);
	fread(&dataLimit, sizeof(int), 1, dataFile);
	int dataNFiles = dataAllPointsNum / dataLimit + 1;

	newAllPointsNum += dataAllPointsNum;
	// write the new number of all points into the index file.
	fseek(indexFile, MAX_FILE_NAME_LENGTH * sizeof(char), SEEK_SET);
	fwrite(&newAllPointsNum, sizeof(Long64T), 1, indexFile);
	fclose(indexFile);

	// get dataset file name of the index
	string indexDataFileName = getWholeFilePath(indexFileName, string(sBuffer));
	FILE* indexDataFile = fopen(indexDataFileName.c_str(), "r+b");
	FAILIF(NULL == indexDataFile);
	fwrite(&newAllPointsNum, sizeof(Long64T), 1, indexDataFile);
	fseek(indexDataFile, 0, SEEK_END);

	if (indexNFiles > 1)
	{
		fclose(indexDataFile);
		string indexDataPiece = indexDataFileName;
		sprintf(buf, "%d", indexNFiles);
		indexDataFile = fopen(indexDataPiece.append(buf).c_str(), "ab");
		FAILIF(NULL == indexDataFile);
	}


	// read all added data file
	char* cpBuf = (char*)MALLOC(dataAllPointsNum * DATASET_ONE_ROW_SIZE);
	char* pcp = cpBuf;
	FAILIF(dataAllPointsNum != fread(cpBuf, DATASET_ONE_ROW_SIZE, dataAllPointsNum, dataFile));
	fclose(dataFile);

	int writePoints = 0;
	while (dataAllPointsNum != 0)
	{
		writePoints = dataAllPointsNum + curPos <= indexLimit ? dataAllPointsNum : indexLimit - curPos;
		FAILIF(writePoints != fwrite(pcp, DATASET_ONE_ROW_SIZE, writePoints, indexDataFile));

		pcp += writePoints;
		dataAllPointsNum -= writePoints;
		curPos = 0;

		fclose(indexDataFile);
		string indexDataPiece = indexDataFileName;
		sprintf(buf, "%d", ++indexNFiles);
		indexDataFile = fopen(indexDataPiece.append(buf).c_str(), "ab");
		FAILIF(NULL == indexDataFile);
	}
	FREE(cpBuf);
	fclose(indexDataFile);
}

void printUHashStructureT(PUHashStructureT hashS, string name)
{
	static int i = 0;
	char buf[12] = {'\0'};
	sprintf(buf, "%d", ++i);
	name.append(buf).append(".txt");
	FILE* fp = fopen(name.c_str(), "wt");
	for (int j = 0; j < 32; j++) {
		fprintf(fp, "%u, %u; ", hashS->mainHashA[j], hashS->controlHash1[j]);
	}
	fprintf(fp, "\n");
	fclose(fp);
}

// for test
void printHashfunction(PRNearNeighborStructT nnStruct, string name)
{
	static int i = 0;
	char buf[12] = {'\0'};
	sprintf(buf, "%d", ++i);
	name.append(buf).append(".txt");
	FILE* fp = fopen(name.c_str(), "wt");

	LSHFunctionT **lshFunctions = nnStruct->lshFunctions;
	// allocate memory for the functions
	// initialize the LSH functions
	fprintf(fp, "lshFunctions:\n");
	for(IntT i = 0; i < nnStruct->nHFTuples; i++){
		fprintf(fp, "fun %d:\n", i);
		for(IntT j = 0; j < nnStruct->hfTuplesLength; j++){
			// vector a
			for(IntT d = 0; d < nnStruct->dimension; d++){
				//fwrite(&lshFunctions[i][j].a[d], sizeof(lshFunctions[i][j].a[d]), 1, indexFile);
				fprintf(fp, "%lf, ", lshFunctions[i][j].a[d]);
			}
			// b
			//fwrite(&lshFunctions[i][j].b, sizeof(lshFunctions[i][j].b), 1, indexFile);
			fprintf(fp, "\nb = %lf\n", lshFunctions[i][j].b);
		}
	}
	fprintf(fp, "mainHashA and controlHash1:\n");
	for (int i = 0; i < nnStruct->parameterL; i++) {
		fprintf(fp, "hash %d of L hashes:\n", i);
		for (int j = 0; j < nnStruct->hashedBuckets[i]->hashedDataLength; j++) {
			//fwrite(&nnStruct->hashedBuckets[i]->mainHashA[j], sizeof(nnStruct->hashedBuckets[i]->mainHashA[j]), 1, indexFile);
			//fwrite(&nnStruct->hashedBuckets[i]->controlHash1[j], sizeof(nnStruct->hashedBuckets[i]->controlHash1[j]), 1, indexFile);
			fprintf(fp, "%u, %u; ", nnStruct->hashedBuckets[i]->mainHashA[j], nnStruct->hashedBuckets[i]->controlHash1[j]);
		}
		fprintf(fp, "\n");
	}

	fclose(fp);
}

void saveHashfunction(PRNearNeighborStructT nnStruct, FILE* indexFile)
{
	fseek(indexFile, DATASET_FILE_INFO_SIZE + ONE_ROW * bucketNum, SEEK_SET);

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

// deprecated
void getHashfunction(PRNearNeighborStructT nnStruct, char* indexFile)
{
	char* current = indexFile + DATASET_FILE_INFO_SIZE + ONE_ROW * bucketNum;
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
	fseek(indexFile, DATASET_FILE_INFO_SIZE + ONE_ROW * bucketNum, SEEK_SET);
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
	PUHashStructureT modelHT = newUHashStructure(HT_LINKED_LIST, hashTableSize, nnStruct->parameterK, TRUE, mainHashA, controlHash1, NULL);

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
				int hIndex = hValue % hashTableSize;

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

	FILE* queryFile = fopen(queryFileName.c_str(), "rb");
	FAILIF(NULL == queryFile);
	FILE* indexFile = fopen(indexFileName.c_str(), "rb");
	FAILIF(NULL == indexFile);
	fread(sBuffer, sizeof(char), MAX_FILE_NAME_LENGTH, indexFile);
	Long64T allPointsNum = 0;
	fread(&allPointsNum, sizeof(Long64T), 1, indexFile);
	fread(&pointNumLimit, sizeof(int), 1, indexFile);
	int nFiles = allPointsNum / pointNumLimit + 1;
	string datasetFileName = getWholeFilePath(indexFileName, string(sBuffer));
	FILE* datasetFile = fopen(datasetFileName.c_str(), "rb");
	FAILIF(NULL == datasetFile);
	int curDataFile = 1; // index of data set files if several data set files exist
	FILE* outputFile = fopen(outputFileName.c_str(), "wb");
	FAILIF(NULL == outputFile);

	printMemory("query1:");
	//IntT resultSize = nQueries;
	//PPointT *result = (PPointT*)MALLOC(resultSize * sizeof(*result));
	PPointT queryPoint;	
	FAILIF(NULL == (queryPoint = (PPointT)MALLOC(sizeof(PointT))));
	FAILIF(NULL == (queryPoint->coordinates = (RealT*)MALLOC(pointsDimension * sizeof(RealT))));

	printMemory("query2:");
	PRNearNeighborStructT nnStruct = getHashedStructure(optParameters, false, true, indexFile);
	//printHashfunction(nnStruct, "E:\\projects\\photodemo\\codes\\PicMatcher\\data\\train\\queryNnStruct");

	printMemory("query3:");
	Uns32T *mainHashA = NULL, *controlHash1 = NULL;
	//Uns32T *mainhashABackup = nnStruct->hashedBuckets[0]->mainHashA;
	PUHashStructureT modelHT = newUHashStructure(HT_LINKED_LIST, hashTableSize, nnStruct->parameterK, TRUE, mainHashA, controlHash1, NULL);
	printMemory("query4:");
	if (mainHashA == NULL) {
		mainHashA = nnStruct->hashedBuckets[0]->mainHashA;
		modelHT->mainHashA = nnStruct->hashedBuckets[0]->mainHashA;
	}

	if (controlHash1 == NULL) {
		controlHash1 = nnStruct->hashedBuckets[0]->controlHash1;
		modelHT->controlHash1 = nnStruct->hashedBuckets[0]->controlHash1;
	}

	//printUHashStructureT(modelHT, "E:\\projects\\photodemo\\codes\\PicMatcher\\data\\train\\QueryHashStr");

	// Uns32T **(precomputedHashesOfULSHs[nnStruct->nHFTuples]);
	Uns32T *** precomputedHashesOfULSHs = (Uns32T ***) MALLOC(nnStruct->nHFTuples * sizeof(Uns32T**));
	for(IntT l = 0; l < nnStruct->nHFTuples; l++){
		FAILIF(NULL == (precomputedHashesOfULSHs[l] = (Uns32T**)MALLOC(sizeof(Uns32T*))));
		for(IntT i = 0; i < 1; i++){
			FAILIF(NULL == (precomputedHashesOfULSHs[l][i] = (Uns32T*)MALLOC(N_PRECOMPUTED_HASHES_NEEDED * sizeof(Uns32T))));
		}
	}
	//printHashfunction(nnStruct, "E:\\projects\\photodemo\\codes\\PicMatcher\\data\\train\\queryNnStruct");
	printMemory("query5:");
	map<Long64T, int> picCount;
	map<int, int> nnPoints;

	Long64T prevFile = -1;
	Long64T currentFile;

	map<pair<int, unsigned int>, int> bucketMatch;

	double temp[IGNORE_DIMENSION] = {0.0};
	char buf[12] = {'\0'};
	fseek(queryFile, sizeof(Long64T) + sizeof(int), SEEK_SET);
	PointId endMark;
	endMark.id = -1;
	endMark.index = -1;
	
	// 每次循环处理图片中的一个点
	while (true) {
		bool isEOF = false;

		if (fread(&(pointId.id), sizeof(Long64T), 1, queryFile) != 1) {
			isEOF = true;
		}
		currentFile = pointId.id;
		fread(&(pointId.index), sizeof(int), 1, queryFile);

		if ((prevFile != -1 && currentFile != prevFile) || isEOF) {
			/*int sBufferSize = 0;
			while (sBuffer[sBufferSize] != '\0') sBufferSize++;
			for (int j = sBufferSize - 1; j >= 0; j--) {
				ungetc(sBuffer[j], queryFile);
			}*/
			fseek(queryFile, -(sizeof(Long64T) + sizeof(int)), SEEK_CUR);

			//std::sort(bucketMatch.begin(), bucketMatch.end());

			//rewind(indexFile);

			fseek(indexFile, DATASET_FILE_INFO_SIZE, SEEK_SET);
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

			if (curDataFile != 1)
			{
				printf("Should not be here this time!\n");
				fclose(datasetFile);
				datasetFile = fopen(datasetFileName.c_str(), "rb");
				FAILIF(NULL == datasetFile);
				curDataFile = 1;
			}

			fseek(datasetFile, sizeof(Long64T) + sizeof(int), SEEK_SET);

			prevIndex = -1;

			int curAllPos = 0;
			for (map<int, int>::iterator it = nnPoints.begin(); it != nnPoints.end(); it++) {
				int curPos = (it->first - prevIndex - 1);
				curAllPos += curPos;
				if (curAllPos >= pointNumLimit)
				{
					printf("Should not be here this time!\n");
					curDataFile += curAllPos / pointNumLimit;
					FAILIF(curDataFile > nFiles);
					fclose(datasetFile);
					sprintf(buf, "%d", curDataFile);
					string datasetPiece = datasetFileName;
					datasetFile = fopen(datasetPiece.append(buf).c_str(), "rb");
					FAILIF(datasetFile == NULL);

					curPos = curAllPos % pointNumLimit;
					curAllPos = curPos;
				}

				fseek(datasetFile, DATASET_ONE_ROW_SIZE * curPos, SEEK_CUR);
				fread(&(pointId.id), sizeof(Long64T), 1, datasetFile);
				fread(&(pointId.index), sizeof(int), 1, datasetFile);
				
				//fread(sBuffer,sizeof(char), MAX_FILE_NAME_LENGTH, datasetFile);
				//string pointsId = string(sBuffer);
				//picCount[pointsId.substr(0, pointsId.find_last_of('_'))] += it->second;
				picCount[pointId.id] += it->second;

				fseek(datasetFile, DATASET_ONE_ROW_SIZE - (sizeof(Long64T) + sizeof(int)), SEEK_CUR);
				prevIndex = it->first;
			}

			vector<pair<int, Long64T> > matchedPicture;
			for (map<Long64T, int>::iterator pos = picCount.begin(); pos != picCount.end(); pos++) {
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
				FAILIF(1 != fwrite(&(matchedPicture[i].second), sizeof(Long64T), 1, outputFile));
				FAILIF(1 != fwrite(&(matchedPicture[i].first), sizeof(int), 1, outputFile));
/*
#ifdef WIN32
				fprintf(outputFile, "%I64d %d ", matchedPicture[i].second, matchedPicture[i].first);
#else
				fprintf(outputFile, "%lld %d ", matchedPicture[i].second, matchedPicture[i].first);
#endif
*/
				
				//printf("%s(%d) ", matchedPicture[i].second.c_str(), matchedPicture[i].first);
			}
			nnPoints.clear();
			picCount.clear();

			FAILIF(1 != fwrite(&(endMark.id), sizeof(Long64T), 1, outputFile));
			FAILIF(1 != fwrite(&(endMark.index), sizeof(int), 1, outputFile));
			//fprintf(outputFile, "\n");
			//printf("\n");

			if (isEOF) break;

			bucketMatch.clear();
		}
		prevFile = currentFile;
		RealT sqrLength = 0;
		// read in the query point.
		IntT d = 0;
		fread(temp, sizeof(double), IGNORE_DIMENSION, queryFile);
		for (d = 0; d < IGNORE_DIMENSION; d++)
		{
			sqrLength += SQR(temp[d]);
		}
		fread(queryPoint->coordinates, sizeof(double), pointsDimension, queryFile);
		for (d = 0; d < pointsDimension; d++)
		{
			sqrLength += SQR(queryPoint->coordinates[d]);
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
				int hIndex = hValue % hashTableSize;

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

	//printHashfunction(nnStruct, "E:\\projects\\photodemo\\codes\\PicMatcher\\data\\train\\queryNnStruct");
	fclose(datasetFile);
	fclose(queryFile);
	fclose(indexFile);
	fclose(outputFile);

	// free queryPoint
	FREE(queryPoint->coordinates);
	FREE(queryPoint);

	// free precomputedHashesOfULSHs
	for(IntT l = 0; l < nnStruct->nHFTuples; l++){
		for(IntT i = 0; i < 1; i++){
			FREE(precomputedHashesOfULSHs[l][i]);
		}
		FREE(precomputedHashesOfULSHs[l]);
	}

	// free modelHT
	FREE(modelHT->hashTable.llHashTable);
	FREE(modelHT);

	// free nnStruct
	FREE(nnStruct->markedPointsIndeces);
	FREE(nnStruct->markedPoints);
	FREE(nnStruct->reducedPoint);
	for(IntT i = 0; i < nnStruct->nHFTuples; i++){
		FREE(nnStruct->precomputedHashesOfULSHs[i]);
		FREE(nnStruct->pointULSHVectors[i]);
	}
	FREE(nnStruct->precomputedHashesOfULSHs);
	FREE(nnStruct->pointULSHVectors);

	for (int i = 0; i < nnStruct->parameterL; i++) {
		FREE(nnStruct->hashedBuckets[i]->mainHashA);
		FREE(nnStruct->hashedBuckets[i]->controlHash1);
		FREE(nnStruct->hashedBuckets[i]);
	}
	FREE(nnStruct->hashedBuckets);

	for(IntT i = 0; i < nnStruct->nHFTuples; i++){
		for(IntT j = 0; j < nnStruct->hfTuplesLength; j++){
			FREE(nnStruct->lshFunctions[i][j].a);
		}
		FREE(nnStruct->lshFunctions[i]);
	}
	FREE(nnStruct->lshFunctions);

	FREE(nnStruct->points);
	FREE(nnStruct);
}

double computeAlpha(string indexFileName) {
	FILE *fp = fopen(indexFileName.c_str(), "rb");

	int cnt = 0;
	fread(sBuffer, sizeof(char), MAX_FILE_NAME_LENGTH, fp);

	for (int i = 0; i < bucketNum; i++) {
		int num;
		fread(&num, sizeof(num), 1, fp);

		fseek(fp, (ONE_ROW - sizeof(int)), SEEK_CUR);

		cnt += num;
	}

	fclose(fp);

	return (double) cnt / ((double) bucketNum * MAX_IN_ONE_BUCKET);
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

	int* ret = (int*) MALLOC(sizeof(int) * num);

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

	fseek(fp, DATASET_FILE_INFO_SIZE + ONE_ROW * bucketIndex, SEEK_SET);

	int num;

	// 得到当前的num计数然后写入新的数据
	fread(&num, sizeof(int), 1, fp);

	if (num < MAX_IN_ONE_BUCKET) {
		fseek(fp, num * ONE_ELEMENT, SEEK_CUR);

		fwrite(&pointIndex, sizeof(int), 1, fp);
		fwrite(&hValue, sizeof(Uns32T), 1, fp);

		num++;

		fseek(fp, DATASET_FILE_INFO_SIZE + ONE_ROW * bucketIndex, SEEK_SET);

		fwrite(&num, sizeof(int), 1, fp);
	} else {
		printf("Some bucket is full, cannot add more points: bucketIndex=%d, pointIndex=%d, hValue=%u.\n", bucketIndex, pointIndex, hValue);
	}

	rewind(fp);
}

/// 初始化索引文件，索引文件首先会包含长度为MAX_FILE_NAME_LENGTH的数据文件名，然后是所有bucket.
void initAllDiskBucket(FILE* fp, string dataSetFileName, Long64T allPointsNum, int pointNumLimit)
{
	FAILIF(fp == NULL);

	rewind(fp);

	fwrite(getFileName(dataSetFileName).c_str(), sizeof(char), MAX_FILE_NAME_LENGTH, fp);
	fwrite(&allPointsNum, sizeof(Long64T), 1, fp);
	fwrite(&pointNumLimit, sizeof(int), 1, fp);

	for (int i = 0; i < bucketNum; i++) {
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

// c:\a\b.c => b.c
string getFileName(const string& filePath)
{
	int i = 0;
	for (i=filePath.length()-1; i>=0; i--)
	{
		if (filePath[i] == '/' || filePath[i] == '\\')
		{
			break;
		}
	}

	return i==-1 ? filePath : filePath.substr(i+1);
} 

// c:\a\b.c, d.e => c:\a\d.e
string getWholeFilePath(const string& filePath, const string& fileName)
{
	int i = 0;
	for (i=filePath.length()-1; i>=0; i--)
	{
		if (filePath[i] == '/' || filePath[i] == '\\')
		{
			break;
		}
	}
	return (i==-1 ? "" : filePath.substr(0, i+1)).append(fileName);
}
