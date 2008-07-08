#include <string.h>
#include <Windows.h>
#include <stdio.h>
#include <string>
#include <time.h>
#include "siftfeat.h"

static int imgDbl = 1;
static double contrThr = 0.04;
static char* keypointFileForMatch = "E:\\projects\\photodemo\\codes\\branches\\TRY-refactor\\bin\\Release\\data\\keypointTest2";
static char* imgFileForTest = "E:\\projects\\photodemo\\codes\\branches\\TRY-refactor\\bin\\Release\\data\\imgFileForTest2.txt";
static int count = 0;
static long totalSiftTime = 0;
static int totalKeypoints = 0;
static double totalFileSize = 0.0;

void doSift(const WIN32_FIND_DATA& findFileData, const char* imgFile, FILE* reFile)
{
	if (!(findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
	{
		// it must NOT be a directory
		printf("The file to be sift is %s, imgDbl=%d, contrThr=%.2f.\n", findFileData.cFileName, imgDbl, contrThr);
		long bt = clock();
		int keypoints = siftImage(imgFile, keypointFileForMatch, imgDbl, contrThr);
		long interval = clock() - bt;
		double fileSize = ((findFileData.nFileSizeHigh * (MAXDWORD+1)) + findFileData.nFileSizeLow) / 1024.0;
		if (keypoints == -1)
		{
			printf("Error in sift file: %s.\n", findFileData.cFileName);
		}
		else
		{
			totalSiftTime += interval;
			totalKeypoints += keypoints;
			totalFileSize += fileSize;
			fprintf(reFile, "%-8d%-9.2f%-7d%-9.1f%-10d%-10ld%-15.1f%-16d%-16ld%s\n", ++count, contrThr, imgDbl, fileSize, keypoints, interval, totalFileSize, totalKeypoints, totalSiftTime, findFileData.cFileName);
		}
	}
}

int testKeypointsByDifferentArguments(int argc, char **argv)
{
	if (argc < 3)
	{
		printf("Usage: PicToBmp img_file_pattern result_file.\n");
		return 1;
	}

	WIN32_FIND_DATA findFileData;
	HANDLE hFind;

	printf("Image file pattern is %s.\n", argv[1]);
	printf("The result file is %s.\n", argv[2]);

	// pick the directory from the source file pattern, i.e.
	// e:\a\* -> e:\a\, e:\a\*.jpg -> e:\a\.
	char sfp[256];
	strcpy(sfp, argv[1]);
	char* p = strrchr(sfp, '\\');
	if (p == 0)
	{
		p = strrchr(sfp, '/');
	}
	if (p != 0)
	{
		*(p+1) = '\0';
	}
	std::string scdir(sfp);
	if (p == 0)
	{
		scdir = "";
	}

	FILE* reFile = fopen(argv[2], "w");
	fprintf(reFile, "sn      contrThr imgDbl fileSize keypoints sift_time total_fileSize total_keypoints total_sift_time file_name\n");

	for (imgDbl=1; imgDbl<=1; ++imgDbl)
	{
		for (int iContrThr=27; iContrThr<100; iContrThr++)
		{
			contrThr = iContrThr / 100.0;

			hFind = FindFirstFile(argv[1], &findFileData);
			if (hFind == INVALID_HANDLE_VALUE)
			{
				printf ("Invalid File Handle. GetLastError reports %d\n.", 
					GetLastError ());
				return 1;
			}

			doSift(findFileData, (scdir + findFileData.cFileName).c_str(), reFile);

			while (FindNextFile(hFind, &findFileData))
			{
				doSift(findFileData, (scdir + findFileData.cFileName).c_str(), reFile);
			}

			FindClose(hFind);
		}
	}

	printf("Convert complete, totally %d files.\n", count);

	return 0;
}

int testKeypointsByDifferentFiles(int argc, char **argv)
{
	if (argc < 3)
	{
		printf("Usage: PicToBmp img_file_pattern result_file.\n");
		return 1;
	}

	WIN32_FIND_DATA findFileData;
	HANDLE hFind;

	printf("Image file pattern is %s.\n", argv[1]);
	printf("The result file is %s.\n", argv[2]);

	// pick the directory from the source file pattern, i.e.
	// e:\a\* -> e:\a\, e:\a\*.jpg -> e:\a\.
	char sfp[256];
	strcpy(sfp, argv[1]);
	char* p = strrchr(sfp, '\\');
	if (p == 0)
	{
		p = strrchr(sfp, '/');
	}
	if (p != 0)
	{
		*(p+1) = '\0';
	}
	std::string scdir(sfp);
	if (p == 0)
	{
		scdir = "";
	}

	hFind = FindFirstFile(argv[1], &findFileData);
	if (hFind == INVALID_HANDLE_VALUE)
	{
		printf ("Invalid File Handle. GetLastError reports %d\n.", 
			GetLastError ());
		return 1;
	}

	FILE* reFile = fopen(argv[2], "w");
	fprintf(reFile, "sn      contrThr imgDbl fileSize keypoints sift_time total_fileSize total_keypoints total_sift_time file_name\n");

	doSift(findFileData, (scdir + findFileData.cFileName).c_str(), reFile);

	while (FindNextFile(hFind, &findFileData))
	{
		doSift(findFileData, (scdir + findFileData.cFileName).c_str(), reFile);
	}

	FindClose(hFind);
	printf("Convert complete, totally %d files.\n", count);

	return 0;
}

int main(int argc, char **argv)
{
	int re;
	//re = testKeypointsByDifferentFiles(argc, argv);
	re = testKeypointsByDifferentArguments(argc, argv);
	return re;
}
