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
//#include <iostream>
//#include "Magick++.h"
//using namespace std;
//using namespace Magick;

/******************************** Globals ************************************/

//extern char img_file_name[30] = "e:\\22\\1.jpg";
//char* out_file_name  = "c:\\≤‚ ‘\\6x_pca3042.txt";
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

/**
 * This interface provides SIFT algorithm implementation. Returns 1 if success, 0 if fail.
**/
extern "C" __declspec(dllexport) int showSift(char* imagenamefile, char* out_file_name, int img_dbl, double contr_thr)
{
	IplImage* img;
	struct feature* features;
	int n = 0, i, j;
	FILE * imageset = fopen(imagenamefile, "rt");
	FILE* outfile = fopen(out_file_name,"w");
	char imagename[255];
	int re = 0;

	while(fgets(imagename, 255, imageset) != NULL)
	{
		int i = 0;
		for (i=0; imagename[i] != '\n' && imagename[i] != '\r' && imagename[i] != '\0'; ++i); // loop stop here
		imagename[i] = '\0';

/*		char   *imagetype=strrchr(imagename,'.');

		if(stricmp(imagetype, ".bmp") != 0)
		{
			Image image;
			cout << imagename << endl;
			image.read(imagename);
			image.magick("bmp");
			char* pp = replace(imagename,imagetype,".bmp");
			image.write(pp);
			strcpy(imagename,pp);

		}
*/
		img = cvLoadImage( imagename, 1 );
		//	fprintf( stderr, "unable to load image from %s", img_file_name );

		if(img == NULL)
		{
			fprintf(stderr, "unable to load image from %s\n", imagename);
			return 0;
		}

		n = _sift_features( img, &features, intvls, sigma, contr_thr, curv_thr,
			img_dbl, descr_width, descr_hist_bins );
		fprintf( stderr, "Found %d features.\n", n);

		for( i = 0; i < n; i++ )
		{
			fprintf( outfile, "%s_%d %f %f %f %f ",imagename, i, features[i].y, features[i].x,
				features[i].scl, features[i].ori );	
			//	fprintf( file, "%s_%d ",img_file_name, i);
			for( j = 0; j < 128; j++ )

				fprintf( outfile, "%d ", ((int)features[i].descr[j] ));

			fprintf( outfile, "\n" );
		}

		cvReleaseImage(&img);

		free(features);
		re = 1;
	}
	fclose(imageset);
	fclose(outfile);

	return re;
}
/*
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd ) 
{
	showSift("imgsForIndex.txt", "output.txt");
}
*/






