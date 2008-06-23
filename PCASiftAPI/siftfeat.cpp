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

#include <highgui.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>

using namespace std;

/******************************** Globals ************************************/

extern char img_file_name[30] = "e:\\22\\1.jpg";
char* out_file_name  = "c:\\≤‚ ‘\\6x_pca3042.txt";
char* img_file_name2 = "..\\10003.bmp";
char* out_file_name2  = "..\\10003.sift";
char dirpath[30] = "c:\\22\\";
int display = 0;
int intvls = SIFT_INTVLS;
double sigma = SIFT_SIGMA;
double contr_thr = SIFT_CONTR_THR;
int curv_thr = SIFT_CURV_THR;
int img_dbl = SIFT_IMG_DBL;
int descr_width = SIFT_DESCR_WIDTH;
int descr_hist_bins = SIFT_DESCR_HIST_BINS;
char* imagenamefile  = "e:\\imagename.txt";
extern int kkll;
extern float eigs[PCASIZE][FEATURE_MAX_D];

/********************************** Main *************************************/
extern "C" __declspec(dllexport) void initialeigs(char* eigsfile)
{
	FILE * pcaf = fopen(eigsfile, "rb"); 
	int ii,jj; 
	//if( eigstest == NULL){
	//	eigstest = calloc( PCASIZE, sizeof( float* ) );
	//	for( i = 0; i < PCASIZE; i++ )
	//	{
	//		eigstest[i] = calloc( FEATURE_MAX_D, sizeof( float ) );
	//	}
	//}
	for (ii = 0; ii < FEATURE_MAX_D; ii++) {
		for (jj = 0; jj < PCASIZE; jj++) {

			float val;
			if (fscanf(pcaf, "%f", &val) != 1) {
				printf("Invalid pca vector file (eig).\n");
				exit(1);
			}                       
			if (jj < PCASIZE)
				eigs[jj][ii] = val;
		}
	}
	kkll = 80;
	printf("initialeigs \n");
	fclose(pcaf);
}


extern "C" __declspec(dllexport) void ShowSift(char* imagenamefile, char* out_file_name, int img_dbl, double contr_thr)
{
	IplImage* img;
	struct feature* features;
	int n = 0, i, j;
	FILE * imageset = fopen(imagenamefile, "rb");
	FILE* outfile = fopen(out_file_name,"w+");
	char imagename[255];

	while(fgets(imagename, 255, imageset) != NULL )
	{
		int index = 0;
		while (imagename[index] != '\n' && imagename[index] != '\0') index++;

		if (imagename[index] == '\n') imagename[index] = '\0';

		index = 0;
		while (imagename[index] != '\r' && imagename[index] != '\0') index++;

		if (imagename[index] == '\r') imagename[index] = '\0';

		img = cvLoadImage( imagename, 1 );
		//	fprintf( stderr, "unable to load image from %s", img_file_name );

		if( ! img )
		{
			fprintf( stderr, "unable to load image from %sasdf\n", imagename );
			getchar();
			exit( 1 );
		}

		n = _sift_features( img, &features, intvls, sigma, contr_thr, curv_thr,
			img_dbl, descr_width, descr_hist_bins );
		fprintf( stderr, "Found %d features.\n", n);

		for( i = 0; i < n; i++ )
		{
			fprintf( outfile, "%s_%d %f %f %f %f ",imagename, i, features[i].y, features[i].x,
				features[i].scl, features[i].ori );	
			//	fprintf( file, "%s_%d ",img_file_name, i);
			for( j = 0; j < 36; j++ )

				fprintf( outfile, "%f ", (features[i].PCAdescr[j]) );

			fprintf( outfile, "\n" );
		}

		cvReleaseImage(&img);

		free(features);
	}
	fclose(imageset);
	fclose(outfile);
}








int main( int argc, char** argv )
{
	IplImage* img;
	struct feature* features;
	int n = 0 , ii, jj, i;
	int start = 0, end = 0;
	FILE * pcaf = fopen("c:\\eigs.txt", "rb");
	char   c[20];   
	int hh;
	int total = 0;
	//if( eigstest == NULL){
	//	eigstest = calloc( PCASIZE, sizeof( float* ) );
	//	for( i = 0; i < PCASIZE; i++ )
	//	{
	//		eigstest[i] = calloc( FEATURE_MAX_D, sizeof( float ) );
	//	}
	//}
	for (ii = 0; ii < FEATURE_MAX_D; ii++) {
		for (jj = 0; jj < PCASIZE; jj++) {

			float val;
			if (fscanf(pcaf, "%f", &val) != 1) {
				printf("Invalid pca vector file (eig).\n");
				exit(1);
			}                       
			if (jj < PCASIZE)
				eigs[jj][ii] = val;
		}
	}
	fclose(pcaf);
	fprintf( stderr, "Finding SIFT features...%s\n",dirpath );

	assert(argc == 3);
	start = atoi(argv[1]);
	end = atoi(argv[2]);
	//	out_file_name = argv[3];
	for(hh = 1; hh <= 1; hh++)
	{
		n = 0;
		itoa(hh,c,10);
		fprintf( stderr, "*********** \n", hh );
		if(hh == 51) strcat(c,".bmp");
		else strcat(c,".jpg");
		strcat(dirpath,c);
		//	strcpy(img_file_name,dirpath);
		strcpy(dirpath,"c:\\22\\");
		img = cvLoadImage( img_file_name, 1 );
		if( ! img )
		{
			fprintf( stderr, "unable to load image from %s", img_file_name );
			getchar();
			exit( 1 );
		}
		n = _sift_features( img, &features, intvls, sigma, contr_thr, curv_thr,
			img_dbl, descr_width, descr_hist_bins );
		fprintf( stderr, "Found %d features in %d image.\n", n, hh );

		if( display )
		{
			draw_features( img, features, n );
			cvNamedWindow( img_file_name, 1 );
			cvShowImage( img_file_name, img );
			cvWaitKey( 0 );
		}

		if( out_file_name != NULL )
			export_features( out_file_name, features, n );

		//		if( out_img_name != NULL )
		//			cvSaveImage( out_img_name, img );
		total += n;
		cvReleaseImage(&img);
		free(features);
	}
	fprintf( stderr, "Found %d features totally!!!!\n", total );
	return 0;
}
