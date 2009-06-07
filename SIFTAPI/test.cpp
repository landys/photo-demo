#include "siftfeat.h"
#include <stdio.h>
#include <stdlib.h>
#include <string>
using namespace std;

string baseDir = "F:\\testpics\\pd_result";
string imgsFile = baseDir + "\\imgs.txt";
string siftBinFile = baseDir + "\\imgs_sift_pruned.dat";
string siftTxtFile = baseDir + "\\imgs_sift_pruned.dat.txt";
string logFileName = baseDir + "\\sift_pruned_time.log";

int main()
{
	printf("%d\n", showSift(imgsFile.c_str(), siftTxtFile.c_str(), 1, 0.04, 100, logFileName.c_str()));
	return 0;
}