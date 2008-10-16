#include "siftfeat.h"
#include "LSHMain.h"
#include <stdio.h>
#include <map>
#include <iostream>
using namespace std;

#define ERROR_OUTPUT stderr
#define FAILIF(b) {if (b) {fprintf(ERROR_OUTPUT, "FAILIF triggered on line %d, file %s.\n", __LINE__, __FILE__); exit(1);}}
#define PRINT_FAIL {fprintf(ERROR_OUTPUT, "FAILIF triggered on line %d, file %s.\n", __LINE__, __FILE__); }
#define FREE(pointer) {if (pointer != NULL) {free(pointer);} pointer = NULL; }
#define MALLOC malloc
const int DATASET_ONE_ROW_SIZE = 1068;

void test()
{
	map<int, int> nnPoints;
	nnPoints[2] += 10;
	nnPoints[5] += 5;
	nnPoints[5] += 2;

	for (map<int, int>::iterator it = nnPoints.begin(); it != nnPoints.end(); it++) {
		printf("point %d=%d\n", it->first, it->second);
	}
}


void printKeypoints(char* dataFileStr, char* dataFileTextStr)
{
	FILE* fp = fopen(dataFileStr, "rb");
	FILE* out = fopen(dataFileTextStr, "wt");

	Long64T id;
	int index;
	double buf[4];
	double data[128];
	int pointLimit = 0;
	int nFiles = 0;
	Long64T allPointsNum = 0;

	fread(&allPointsNum, sizeof(Long64T), 1, fp);
	printf("allPointsNum=%lld\n", allPointsNum);
	fread(&pointLimit, sizeof(int), 1, fp);
	printf("pointLimit=%d\n", pointLimit);
	nFiles = allPointsNum / pointLimit + 1;
	printf("nFiles=%d\n", nFiles);
	while (fread(&id, sizeof(Long64T), 1, fp) > 0)
	{
		fprintf(out, LONG64T_TEXT, id);
		fread(&id, sizeof(int), 1, fp);
		fprintf(out, "_%d", id);
		fread(&buf, sizeof(double), 4, fp);
		int i=0; 
		for (i=0; i<4; i++)
		{
			fprintf(out, " %lf", buf[i]);
		}

		fread(&data, sizeof(double), 128, fp);
		for (i=0; i<128; i++)
		{
			fprintf(out, " %lf", data[i]);
		}
		fprintf(out, "\n");
	}
	fclose(fp);
	fclose(out);
}

void testSiftApi()
{
	//int n = siftImage("E:\\projects\\photodemo\\codes\\PicMatcher\\data\\tmpHeadImg\\1.jpg", "E:\\projects\\photodemo\\codes\\PicMatcher\\data\\train\\test_keypoints", 1, 0.04, 33);
	//int n = showSift("E:\\projects\\photodemo\\codes\\PicMatcher\\data\\train\\index_img_files", "E:\\projects\\photodemo\\codes\\PicMatcher\\data\\train\\test_keypoints", 1, 0.04);
	//printf("n=%d\n", n);

	printKeypoints("E:\\projects\\photodemo\\codes\\PicMatcher\\data\\train\\index_keypoints",
		"E:\\projects\\photodemo\\codes\\PicMatcher\\data\\train\\index_keypoints_text");

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

void testGetFileName()
{
	cout << "RE:" << getFileName("c:\\a\\b.c").c_str() << endl;
	cout << "RE:" << getFileName("c:/a/b.c").c_str() << endl;
	cout << "RE:" << getFileName("c:/a/").c_str() << endl;
	cout << "RE:" << getFileName("b.c").c_str() << endl;
	cout << "RE:" << getFileName("/").c_str() << endl;
	cout << "RE:" << getFileName("/b.c").c_str() << endl;
}

void testGetWholeFilePath()
{
	cout << "RE:" << getWholeFilePath("c:\\a\\b.c", "d.e").c_str() << endl;
	cout << "RE:" << getWholeFilePath("c:/a/b.c", "").c_str() << endl;
	cout << "RE:" << getWholeFilePath("c:/a/", "d.e").c_str() << endl;
	cout << "RE:" << getWholeFilePath("b.c", "d.e").c_str() << endl;
	cout << "RE:" << getWholeFilePath("/", "d.e").c_str() << endl;
	cout << "RE:" << getWholeFilePath("/b.c", "d.e").c_str() << endl;
}

const int MAX_FILE_NAME_LENGTH = 256;
char sBuffer[1000];
int bucketNum = 50000;
const int ONE_ELEMENT = sizeof(int) + sizeof(unsigned);
const int MAX_IN_ONE_BUCKET = 200;

// no use here, may be wrong as index file structure has be changed
// deprecated
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
		//printf("Output bucket %d\n.", i);
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

void printIndexInfo(string indexName)
{
	FILE *fp = fopen(indexName.c_str(), "rb");
	fread(sBuffer, sizeof(char), MAX_FILE_NAME_LENGTH, fp);
	Long64T allPointsNum;
	int pointNumLimit;
	fread(&allPointsNum, sizeof(Long64T), 1, fp);
	fread(&pointNumLimit, sizeof(int), 1, fp);
	printf("index: %s, %I64d, %d\n", sBuffer, allPointsNum, pointNumLimit);
	fclose(fp);
}

void printOutput(string outName, string outtextName)
{
	FILE* out = fopen(outName.c_str(), "rb");
	FILE* outText = fopen(outtextName.c_str(), "wt");
	Long64T id;
	int index;
	while (fread(&id, sizeof(Long64T), 1, out) == 1)
	{
		fread(&index, sizeof(int), 1, out);
		if (id == -1 && index == -1)
		{
			fprintf(outText, "\n");
		}
		else
		{
			fprintf(outText, "%I64d %d, ", id, index);
		}
	}
	fclose(out);
	fclose(outText);
}

void testE2LSH()
{
	/*
	int n1 = siftImage("E:\\projects\\photodemo\\codes\\PicMatcher\\data\\tmpHeadImg\\1.jpg", "E:\\projects\\photodemo\\codes\\PicMatcher\\data\\train\\test_keypoints", 1, 0.04, 55);
	int n2 = showSift("E:\\projects\\photodemo\\codes\\PicMatcher\\data\\train\\index_img_files", "E:\\projects\\photodemo\\codes\\PicMatcher\\data\\train\\index_keypoints", 1, 0.04);
	int n3 = siftImage("E:\\projects\\photodemo\\codes\\PicMatcher\\data\\tmpHeadImg\\3.jpg", "E:\\projects\\photodemo\\codes\\PicMatcher\\data\\train\\add_keypoints", 1, 0.04, 44);
	//printf("n3=%d\n", n3);
	//printf("n1=%d, n2=%d\n", n1, n2);

	printf("Begin to set up index\n");
	setUpIndex("E:\\projects\\photodemo\\codes\\PicMatcher\\data\\train\\index_keypoints", "E:\\projects\\photodemo\\codes\\PicMatcher\\data\\train\\myindex");
	printIndexInfo("E:\\projects\\photodemo\\codes\\PicMatcher\\data\\train\\myindex");
	outputIndexFile("E:\\projects\\photodemo\\codes\\PicMatcher\\data\\train\\myindex", "E:\\projects\\photodemo\\codes\\PicMatcher\\data\\train\\myindex.txt");
	printf("Begin add to index\n");
	query("E:\\projects\\photodemo\\codes\\PicMatcher\\data\\train\\test_keypoints", "E:\\projects\\photodemo\\codes\\PicMatcher\\data\\train\\myindex", "E:\\projects\\photodemo\\codes\\PicMatcher\\data\\train\\test1");
	printOutput("E:\\projects\\photodemo\\codes\\PicMatcher\\data\\train\\test1", "E:\\projects\\photodemo\\codes\\PicMatcher\\data\\train\\test1.txt");
	printKeypoints("E:\\projects\\photodemo\\codes\\PicMatcher\\data\\train\\index_keypoints",
		"E:\\projects\\photodemo\\codes\\PicMatcher\\data\\train\\index_keypoints.txt");

	addToIndex("E:\\projects\\photodemo\\codes\\PicMatcher\\data\\train\\add_keypoints", "E:\\projects\\photodemo\\codes\\PicMatcher\\data\\train\\myindex");
	printIndexInfo("E:\\projects\\photodemo\\codes\\PicMatcher\\data\\train\\myindex");
	printKeypoints("E:\\projects\\photodemo\\codes\\PicMatcher\\data\\train\\index_keypoints",
		"E:\\projects\\photodemo\\codes\\PicMatcher\\data\\train\\index_keypoints_added.txt");
	//printf("end add to index\n");
	outputIndexFile("E:\\projects\\photodemo\\codes\\PicMatcher\\data\\train\\myindex", "E:\\projects\\photodemo\\codes\\PicMatcher\\data\\train\\myindex_added.txt");
	printf("Begin to query\n");
	
	query("E:\\projects\\photodemo\\codes\\PicMatcher\\data\\train\\test_keypoints", "E:\\projects\\photodemo\\codes\\PicMatcher\\data\\train\\myindex", "E:\\projects\\photodemo\\codes\\PicMatcher\\data\\train\\test2");
	printOutput("E:\\projects\\photodemo\\codes\\PicMatcher\\data\\train\\test2", "E:\\projects\\photodemo\\codes\\PicMatcher\\data\\train\\test2.txt");
	
	query("E:\\projects\\photodemo\\codes\\PicMatcher\\data\\train\\add_keypoints", "E:\\projects\\photodemo\\codes\\PicMatcher\\data\\train\\myindex", "E:\\projects\\photodemo\\codes\\PicMatcher\\data\\train\\out");
	printOutput("E:\\projects\\photodemo\\codes\\PicMatcher\\data\\train\\out", "E:\\projects\\photodemo\\codes\\PicMatcher\\data\\train\\out.txt");
*/
	printKeypoints("E:\\projects\\photodemo\\codes\\branches\\TRY-refactor-jni-envelop\\bin\\Release\\data\\add_keypoints",
		"E:\\projects\\photodemo\\codes\\branches\\TRY-refactor-jni-envelop\\bin\\Release\\data\\add_keypoints.txt");

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
	printf("end of add dataset to index dataset\n");
}

void testAddDataSetToIndexDataSet()
{
	FILE *fp = fopen("E:\\projects\\photodemo\\codes\\PicMatcher\\data\\train\\myindex", "rb");
	fread(sBuffer, sizeof(char), MAX_FILE_NAME_LENGTH, fp);
	Long64T allPointsNum;
	int pointNumLimit;
	fread(&allPointsNum, sizeof(Long64T), 1, fp);
	fread(&pointNumLimit, sizeof(int), 1, fp);
	printf("index: %s, %I64d, %d\n", sBuffer, allPointsNum, pointNumLimit);
	fclose(fp);

	printKeypoints("E:\\projects\\photodemo\\codes\\PicMatcher\\data\\train\\index_keypoints",
		"E:\\projects\\photodemo\\codes\\PicMatcher\\data\\train\\index_keypoints_text.txt");
	addDataSetToIndexDataSet("E:\\projects\\photodemo\\codes\\PicMatcher\\data\\train\\add_keypoints", "E:\\projects\\photodemo\\codes\\PicMatcher\\data\\train\\myindex");
	fp = fopen("E:\\projects\\photodemo\\codes\\PicMatcher\\data\\train\\myindex", "rb");
	fread(sBuffer, sizeof(char), MAX_FILE_NAME_LENGTH, fp);
	fread(&allPointsNum, sizeof(Long64T), 1, fp);
	fread(&pointNumLimit, sizeof(int), 1, fp);
	printf("index: %s, %I64d, %d\n", sBuffer, allPointsNum, pointNumLimit);
	fclose(fp);
	printKeypoints("E:\\projects\\photodemo\\codes\\PicMatcher\\data\\train\\index_keypoints",
		"E:\\projects\\photodemo\\codes\\PicMatcher\\data\\train\\index_keypoints_text2.txt");
}

int main()
{
	//testSiftApi();
	//testGetFileName();
	//testGetWholeFilePath();
	testE2LSH();
	//printKeypoints("E:\\projects\\photodemo\\codes\\PicMatcher\\data\\train\\index_keypoints",
	//	"E:\\projects\\photodemo\\codes\\PicMatcher\\data\\train\\index_keypoints_text.txt");
	//testAddDataSetToIndexDataSet();

	return 0;
}