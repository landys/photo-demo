#include "siftfeat.h"
#include "LSHMain.h"
#include <stdio.h>
#include <map>
#include <iostream>
using namespace std;

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

void testSiftApi()
{
	//int n = siftImage("E:\\projects\\photodemo\\codes\\PicMatcher\\data\\tmpHeadImg\\1.jpg", "E:\\projects\\photodemo\\codes\\PicMatcher\\data\\train\\test_keypoints", 1, 0.04, 33);
	//int n = showSift("E:\\projects\\photodemo\\codes\\PicMatcher\\data\\train\\index_img_files", "E:\\projects\\photodemo\\codes\\PicMatcher\\data\\train\\test_keypoints", 1, 0.04);
	//printf("n=%d\n", n);

	FILE* fp = fopen("E:\\projects\\photodemo\\codes\\PicMatcher\\data\\train\\index_keypoints", "rb");
	FILE* out = fopen("E:\\projects\\photodemo\\codes\\PicMatcher\\data\\train\\index_keypoints_text", "wt");
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

void testE2LSH()
{
	//int n1 = siftImage("E:\\projects\\photodemo\\codes\\PicMatcher\\data\\tmpHeadImg\\1.jpg", "E:\\projects\\photodemo\\codes\\PicMatcher\\data\\train\\test_keypoints", 1, 0.04);
	//int n2 = showSift("E:\\projects\\photodemo\\codes\\PicMatcher\\data\\train\\index_img_files", "E:\\projects\\photodemo\\codes\\PicMatcher\\data\\train\\index_keypoints", 1, 0.04);
	//printf("n1=%d, n2=%d\n", n1, n2);

	//printf("Begin to set up index\n");
	//setUpIndex("E:\\projects\\photodemo\\codes\\PicMatcher\\data\\train\\index_keypoints", "E:\\projects\\photodemo\\codes\\PicMatcher\\data\\train\\myindex");
	//outputIndexFile("E:\\projects\\photodemo\\codes\\PicMatcher\\data\\train\\myindex", "E:\\projects\\photodemo\\codes\\PicMatcher\\data\\train\\myindex.txt");
	printf("Begin to query\n");
	query("E:\\projects\\photodemo\\codes\\PicMatcher\\data\\train\\test_keypoints", "E:\\projects\\photodemo\\codes\\PicMatcher\\data\\train\\myindex", "E:\\projects\\photodemo\\codes\\PicMatcher\\data\\train\\out");
	FILE* out = fopen("E:\\projects\\photodemo\\codes\\PicMatcher\\data\\train\\out", "rb");
	FILE* outText = fopen("E:\\projects\\photodemo\\codes\\PicMatcher\\data\\train\\out.txt", "wt");
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

int main()
{
	testSiftApi();
	//testGetFileName();
	//testGetWholeFilePath();
	testE2LSH();
	return 0;
}