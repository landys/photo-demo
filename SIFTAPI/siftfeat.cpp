/*
This program detects image features using SIFT keypoints. For more info,
refer to:

Lowe, D. Distinctive image features from scale-invariant keypoints.
International Journal of Computer Vision, 60, 2 (2004), pp.91--110.

Copyright (C) 2006  Rob Hess <hess@eecs.oregonstate.edu>

Note: The SIFT algorithm is patented in the United States and cannot be
used in commercial products without a license from the University of
British Columbia.  For more information, refer to the file LICENSE.ubc
that accompanied this distribution.

Version: 1.1.1-20070330
*/

#include "sift.h"
#include "imgfeatures.h"
#include "utils.h"
#include "siftfeat.h"

#include <highgui.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
//#include <iostream>
//#include "Magick++.h"
//using namespace std;
//using namespace Magick;

/******************************** Globals ************************************/

#define ERROR_OUTPUT stderr
#define FAILIF(b) {if (b) {fprintf(ERROR_OUTPUT, "FAILIF triggered on line %d, file %s.\n", __LINE__, __FILE__); exit(1);}}
#define PRINT_FAIL {fprintf(ERROR_OUTPUT, "FAILIF triggered on line %d, file %s.\n", __LINE__, __FILE__); }
#define FREE(pointer) {if (pointer != NULL) {free(pointer);} pointer = NULL; }

//extern char img_file_name[30] = "e:\\22\\1.jpg";
//char* out_file_name  = "c:\\����\\6x_pca3042.txt";
//char* img_file_name2 = "..\\10003.bmp";
//char* out_file_name2  = "..\\10003.sift";
//char dirpath[30] = "c:\\22\\";
//int display = 0;
int intvls = SIFT_INTVLS;
double sigma = SIFT_SIGMA;
double contr_thr = SIFT_CONTR_THR;
int curv_thr = SIFT_CURV_THR;
int img_dbl = SIFT_IMG_DBL;
int descr_width = SIFT_DESCR_WIDTH;
int descr_hist_bins = SIFT_DESCR_HIST_BINS;
//char* imagenamefile  = "e:\\imagename.txt";
char* logFileName = "sift.log";
int curPointNum = 0;
long long allPointsNum = 0; // the number of points of all images
int nFileIndex = 0; // the index of files if the result saved in several files
FILE* outfile = 0;	// the FILE of the result
char outfileName[256] = {'\0'};	// the result file name

// one point = long long+int+double(4)+double(128) = 1036 bytes.
// the file should be less than 2G, so a file should contain less than 2072860 points.
// the file may contains some other information about the data, so the limit of points is set as 2000000.
const int LIMIT_POINTS_PER_FILE = 2000000;

int doSiftImage(const char* imagename, struct feature** ppfeatures, int img_dbl, double contr_thr, int n_max);
void saveOneImageFeatures(struct feature* pfeatures, int n, long long id);
void saveAndCloseOutFileHeader();
void initParameter(const char* out_file_name);

/**
 * This interface provides SIFT algorithm implementation. Returns number of files to be sifted if success, -1 if fail.
 * The image names in showSift should be less then 255, or maybe stack overflow.
**/
extern "C" DLL_EXPORT int showSift(const char* imagenamefile, const char* out_file_name, int img_dbl, double contr_thr, int n_max/*=0*/)
{
	initParameter(out_file_name);

	int nFiles = 0;

	FILE * imageset = fopen(imagenamefile, "rt");
	FAILIF(imageset == NULL);
	outfile = fopen(outfileName,"w+b");
	FAILIF(outfile == NULL);
	fseek(outfile, sizeof(long long)+sizeof(int), SEEK_SET); // for file header about the datasets
	char linebuf[256] = {'\0'};
	char* imagename = 0;
	
	long long id = 0;
	struct feature* pfeatures = 0;

	//long abt = clock();
	while(fscanf(imageset, "%lld", &id) != EOF)
	{
		fgets(linebuf, 255, imageset);
		int i = 0;
		for (i=0; linebuf[i] != '\n' && linebuf[i] != '\r' && linebuf[i] != '\0'; ++i); // loop stop here
		for (--i; i>=0 && (linebuf[i] == ' ' || linebuf[i] == '\t'); --i); // loop stop here, eliminate the tail spaces/tabs.
		if (i < 0)
		{
			// not legal file name with only spaces/tabs.
			continue;
		}
		linebuf[i+1] = '\0';
		for (imagename=linebuf; *imagename!='\0' && (*imagename==' ' || *imagename=='\t'); ++imagename); // loop stop here, eliminate the head spaces/tabs.

		int n = doSiftImage(imagename, &pfeatures, img_dbl, contr_thr, n_max);
		if (n == -1)
		{
			FREE(pfeatures);
			continue;
		}
		saveOneImageFeatures(pfeatures, n, id);
		FREE(pfeatures);

		allPointsNum += n;
		++nFiles;
	}
	fclose(imageset);

	saveAndCloseOutFileHeader();

	return nFiles;
}

/**
 * This interface provides SIFT algorithm implementation. Returns number of keypoints if success, -1 if fail.
 * @parameter imagename the file name of the image
 * @parameter out_file_name the output keypoints file
 * 
 */
extern "C" DLL_EXPORT int siftImage(const char* imagename, const char* out_file_name, int img_dbl, double contr_thr, long long id/*=0*/, int n_max/*=0*/)
{
	initParameter(out_file_name);

	outfile = fopen(outfileName, "w+b");
	FAILIF(outfile == NULL);
	fseek(outfile, sizeof(long long)+sizeof(int), SEEK_SET); // for file header about the datasets

	struct feature* pfeatures = 0;

	int n = doSiftImage(imagename, &pfeatures, img_dbl, contr_thr, n_max);
	if (n > 0) {
		saveOneImageFeatures(pfeatures, n, id);
		allPointsNum += n;
	}
	FREE(pfeatures);

	saveAndCloseOutFileHeader();
	
	return n;

}

/**
 * Do real sift here, and save the featured into an opened file, which passed from the caller.
**/
int doSiftImage(const char* imagename, struct feature** ppfeatures, int img_dbl, double contr_thr, int n_max)
{
	IplImage* img;
	
	img = cvLoadImage( imagename, 1 );
	//	fprintf( stderr, "unable to load image from %s", img_file_name );

	if(img == NULL)
	{
		fprintf(stderr, "unable to load image from %s, on line %d, file %s.\n", imagename, __LINE__, __FILE__);
		return -1;
	}

	int n = _sift_features( img, ppfeatures, intvls, sigma, contr_thr, curv_thr,
		img_dbl, descr_width, descr_hist_bins, n_max );

	cvReleaseImage(&img);

	return n;
}

// save the features of one point into the outfile, return the number of points actually saved.
void saveOneImageFeatures(struct feature* pfeatures, int n, long long id)
{
	static char buf[12] = {'\0'};
	static char outfilePiece[256] = {'\0'};

	for(int i = 0; i < n; i++ )
	{
		if (++curPointNum > LIMIT_POINTS_PER_FILE)
		{
			curPointNum = 1;
			fclose(outfile);
			sprintf(buf, "%d", ++nFileIndex);
			strcpy(outfilePiece, outfileName);
			strcat(outfilePiece, buf);
			outfile = fopen(outfilePiece, "wb");
			FAILIF(outfile == NULL);
		}
		//fprintf( outfile, "%s_%d %f %f %f %f ",imagename, i, pfeatures[i].y, pfeatures[i].x,
		//	pfeatures[i].scl, pfeatures[i].ori );
		fwrite(&id, sizeof(long long), 1, outfile);
		fwrite(&i, sizeof(int), 1, outfile);
		fwrite(&(pfeatures[i].y), sizeof(double), 1, outfile);
		fwrite(&(pfeatures[i].x), sizeof(double), 1, outfile);
		fwrite(&(pfeatures[i].scl), sizeof(double), 1, outfile);
		fwrite(&(pfeatures[i].ori), sizeof(double), 1, outfile);
		//for(int j = 0; j < 128; j++ )
		//{
		//	fprintf( outfile, "%d ", ((int)pfeatures[i].descr[j] ));	
		//}
		fwrite(&(pfeatures[i].descr[0]), sizeof(double), FEATURE_MAX_D, outfile);

		//fprintf( outfile, "\n" );
	}

}

void saveAndCloseOutFileHeader()
{
	if (nFileIndex >= 2)
	{
		fclose(outfile);
		outfile = fopen(outfileName,"r+b");
		FAILIF(outfile == NULL);
	}

	rewind(outfile);
	fwrite(&allPointsNum, sizeof(long long), 1, outfile);
	fwrite(&LIMIT_POINTS_PER_FILE, sizeof(int), 1, outfile);
	fclose(outfile);
}

void initParameter(const char* out_file_name)
{
	curPointNum = 0;
	allPointsNum = 0;
	nFileIndex = 1;
	strcpy(outfileName, out_file_name);
}

/*
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd ) 
{
	siftImage("E:\\projects\photodemo\\codes\\PicMatcher\\data\\tmpHeadImg\\86958406.jpg", "E:\\projects\photodemo\\codes\\PicMatcher\\data\\train\\test_keypoints", 1, 0.04, 12345678901234);

	return 0;
}
*/

