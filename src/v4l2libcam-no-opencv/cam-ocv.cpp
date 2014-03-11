
/*
 * Copyright (C) 2009 Giacomo Spigler
 * CopyPolicy: Released under the terms of the GNU GPL v3.0.
 */

//Single webcam demo

#include <iostream>
#include <cv.h>
#include <highgui.h>
#include <stdio.h>
#include <vector>

using namespace std;

#define USE_OPENCV 1

//#include "../libcam.h"
#include "libcam.h"
#include "ipl.h"

using namespace std;

int main(int argc, char **args) {
	if(argc > 1 && argc < 3) {
		frame myFrame;
		int X = 3, Y = 4;
		unsigned char *data_1d, **data_2d;
		data_1d = (unsigned char*)malloc(sizeof(unsigned char)*Y);
		for(int y = 0; y < Y; ++y)
			 data_1d[y] = 10;

		data_2d = (unsigned char **)malloc(sizeof(unsigned char)*Y);	// inisialisasi banyaknya baris
		for(int y = 0; y< Y;y++) // inisialisasi banyaknya kolom tiap baris
			data_2d[y] = (unsigned char *)malloc(sizeof(unsigned char)*X);

		for(int y = 0; y < Y; ++y){
			for(int x = 0; x < X; ++x)
				data_2d[y][x] = 25;
		}
		int i(0);
		for(int y = 0; y < Y; ++y) {
			for(int x = 0; x < X; ++x) {
				++i;
				printf("%i->(%i,%i):\t%i\n", i, y, x, (int)data_2d[y][x]);
			}
		}

		// array size
		float *f;
		f = (float*)malloc(3);

		cout << f[0] << endl;
		//f = (float *)malloc(sizeof(float)*1);
		free(f);
		cout << "after free: " << f[6] << endl;

		return 0;
	}


	int ww=640;
	int hh=480;
	int fps=30;

	const char *dev="/dev/video0";

	printf("Usage is:\n%s -w width -h height -d device -f fps\n\n", args[0]);


	// parsing arguments
	for(int i=1; i<argc-1; i++) {
		string a=args[i];
		if(a=="-w") {
			ww=atoi(args[i+1]);
		} else if(a=="-h") {
			hh=atoi(args[i+1]);
		} else if(a=="-d") {
			dev=args[i+1];
		} else if(a=="-f") {
			fps=atoi(args[i+1]);
		}
	}


	// 1) Instance a Camera object
	Camera c(dev, ww, hh, fps);
	if(c.data == NULL)
	{
		fprintf(stderr, "EXIT! data=NULL");
		exit(1);
	}
	cvNamedWindow("l", CV_WINDOW_AUTOSIZE);

	IplImage *l=cvCreateImage(cvSize(ww, hh), 8, 3);
//	printf("FINISH");
	RGB value;
	myImage *I, i;
	I = &i;
	while(1){
		// 2) Grab next frame
		c.Update();

		// 3) Convert to OpenCV format  (default is YUYV, stored into c.data[] )
		c.toIplImage(l);

		/* [BEGIN] my class */
		c.setToImage(I);
		IMAGING img(I->w, I->h, I->data, I->nc);
		cout << img.channels << endl;
		/* [END] my class */

		cvShowImage("l", l);

//		value = c.at(100, 200);	// ini yang benar
//			printf("-->[%i %i %i]\n", (int)value.r, (int)value.g, (int)value.b);
		if( (cvWaitKey(10) & 255) == 27 ) {
			//img.Clear();
			c.StopCam();
			break;
		}
	}

	printf("STOPCAM()\n");
	cvDestroyWindow("l");
	cvReleaseImage(&l);


	// 4) Automatic cleanup is done when the app terminates

	return 0;
}
