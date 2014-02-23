/*
 * ipl.h
 *
 *  Created on: Feb 2, 2014
 *      Author: lynx
 */

#ifndef IPL_H_
#define IPL_H_

#include "types.h"
#include <malloc.h>
#include <alloca.h>
#include <stdlib.h>
#include <stdint-gcc.h>
#include <stdbool.h>

#if USE_OPENCV
#include <cv.h>
#include <highgui.h>
#endif

using namespace std;
// processing usually needs better data format e.g uchar, float, int, double, etc.
typedef enum {
	IMG_FMT_UCHAR,
	IMG_FMT_INT,
	IMG_FMT_FLOAT,
	IMG_FMT_DBL,
	IMG_FMT_LONG_INT
} datatypes ;

typedef enum {
	CHANNEL_1,	// grayscale
	CHANNEL_3,	// rgb, hsv, yuv, hsl
	CHANNEL_4	// cmyk, yuyv
} nchannel;

typedef struct {
	unsigned char r, g, b;
}RGB;

typedef enum {
	FILTER_GAUSSIAN,
	FILTER_MEDIAN
}filterName;

typedef struct {
	int w, h, nc;
	unsigned char* data;
} myImage;

typedef struct {
	int w, h;
	void print(void) {
		printf("[%i %i]", w, h);
	}
}mySize;

/* this class mainly using RGB color space */
class IMAGING {
public:
	IMAGING(int width, int height, unsigned char* data, int nChannels = 1);
	IMAGING(myImage *I) ;
	int width() 	const { return w; }
	int height() 	const { return h; }
	template<typename _datatype>
	_datatype* at(int x, int y) const {
		_datatype *d;
		d = (_datatype *)malloc(sizeof(_datatype)*nc);
		int i=( (y*w/2) + x)*nc;
		for(int idx = 0; idx < nc; ++idx) {
			d[idx] = p[i+idx];
		}
		return d;
	}

	RGB GetRGB(int x, int y) const;
	mySize size;
	int channels;
	void Clear();
	/*virtual*/ ~IMAGING();
private:
	int w, h, nc;
	unsigned char *p;

	unsigned char *rgb_data;

protected:
//#ifdef _RGB_SPACE_
	unsigned char yv[256][256];
	unsigned char yu[256][256];
			 int y2v[256][256];
			 int y2u[256][256];
	/*
	 * output data format is {[RGB],[RGB]...}
	 */
	unsigned char* GenerateRGB() ;
//#endif
};



/*
#if USE_OPENCV
class IPL_IMAGE {
public:
	IPL_IMAGE( IplImage &I) {
		i_ = &I;
		RGBflag = false;
	}
	*
	 * RGB data format is used!
	 *
	RGB at(int x, int y) {
		if(RGBflag) {
			v.b = 0; v.g=0; v.r=0;
			return v;
		} else {
			v.b = i_->imageData[3*(y*i_->width+x) + 0];
			v.g = i_->imageData[3*(y*i_->width+x) + 1];
			v.r = i_->imageData[3*(y*i_->width+x) + 2];
			RGBflag = true;
			return v;
		}
	}

	void print() {
		if(RGBflag)
		{
			printf("[%i %i %i]\n", (int)v.r, (int)v.g, (int)v.b);
			RGBflag = false;
			return;
		} else
			return;
	}
private:
	RGB v;
	bool RGBflag;
	IplImage *i_;
};
#endif
*/

#endif /* IPL_H_ */
