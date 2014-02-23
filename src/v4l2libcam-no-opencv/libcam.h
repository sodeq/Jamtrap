/*
 * Copyright (C) 2009 Giacomo Spigler
 * 2013 - George Jordanov - improve in performance for HSV conversion and improvements
 *
 * CopyPolicy: Released under the terms of the GNU GPL v3.0.
 */

#ifndef __LIBCAM__H__
#define __LIBCAM__H__

#define USE_OPENCV 1
#define USE_LOOKUP 1

#ifdef USE_OPENCV
#include <cv.h>
#include "opencv2/core/core.hpp"
#endif

#include "ipl.h"

struct buffer {
        void *                  start;
        size_t                  length;
};

typedef enum {
	IO_METHOD_READ,
	IO_METHOD_MMAP,
	IO_METHOD_USERPTR
} io_method ;

typedef enum {
	COLOR_YUYV,
	COLOR_RGB
} colormodel;


/****************************************************************
 * NOTE:					Camera
 * Camera: 	Open and give a faster and easier data access and
 * 			inter-operability.
 * 			Further processing algorithm is highly necessary.
 * 			1. For display purpose, you can use option: -D WITH_OPENCV
 * TODO:
 * 	1. Need to align *data into : matrix like data
 * 	2. Better YUYV to RGB conversion is necessary.
 ****************************************************************/
class Camera {
// NOTE: PRIVATE
private:
  void Open();
  void Close();

  void Init();
  void UnInit();

  void Start();
  void Stop();

  void init_userp(unsigned int buffer_size);
  void init_mmap();
  void init_read(unsigned int buffer_size);

  bool initialised;
// NOTE: USE_LOOKUP
#ifdef USE_LOOKUP
	void genYUVtoRGBLookups();
	unsigned char yv[256][256];
	unsigned char yu[256][256];
			 int y2v[256][256];
			 int y2u[256][256];
#endif
  /* return is formatted as:
   * 	YUYV: 	[Y][U][Y][V]
   * 	RGB:	[R][G][B]
   */
  unsigned char* readAt(int x, int y, colormodel cm = COLOR_RGB) const {
	  unsigned char* a;

	  int y0, y1, u, v;
	  int i=(y*w2+x)*4;	// jika menggunakan teknik pengisian tiap kolom
	  y0=data[i];
	  u=data[i+1];
	  y1=data[i+2];
	  v=data[i+3];

/**
 * FIXME: URGENT!
 * NOTE: DONE 19-02-2014	(Z)
 * jika menggunakan pengisian tiap baris
 **/
	  /*a[0] = data[width * x + y + 0];	// Y+
	  a[1] = data[width * x + y + 1];	// U
	  a[2] = data[width * x + y + 2];	// Y-
	  a[3] = data[width * x + y + 3];	// V
	  */
	  switch (cm) {
		case COLOR_YUYV:
			a = (unsigned char *)malloc(sizeof(unsigned char)*4);
			a[0] = (unsigned char)y0;
			a[1] = (unsigned char)u;
			a[2] = (unsigned char)y1;
			a[3] = (unsigned char)v;
			break;

		case COLOR_RGB:
			int r, g, b;
			a = (unsigned char *)malloc(sizeof(unsigned char)*3);
			r = y0 + (1.370705 * (v - 128));
			g = y0 - (0.698001 * (v - 128))-(0.337633 * (u-128));
			b = y0 + (1.732446 * (u-128));

			if( r > 255) r = 255;
			if(g > 255) g = 255;
			if(b > 255) b = 255;
			if(r < 0) r=0;
			if(g < 0) g=0;
			if(b < 0) b=0;
			a[0] = (unsigned char)r;
			a[1] = (unsigned char)g;
			a[2] = (unsigned char)b;

			break;
		default:
			break;
	}
	  return a;
  }

// NOTE: PUBLIC
public:
  const char *name;  //dev_name
  int width;
  int height;
  int fps;
  int channels;
  int w2;

  unsigned char *data;

  io_method io;
  int fd;
  buffer *buffers;
  int n_buffers;

  int mb, Mb, db, mc, Mc, dc, ms, Ms, ds, mh, Mh, dh, msh, Msh, dsh;
  bool ha;

  //Camera();
  Camera(const char *name, int w, int h, int fps=30);
  void  StartCamera(const char *name, int w, int h, int fps=30);
  ~Camera();

  unsigned char *Get();    //deprecated
  bool Update(unsigned int t=100, int timeout_ms=500); //better  (t=0.1ms, in usecs)
  bool Update(Camera *c2, unsigned int t=100, int timeout_ms=500);

// NOTE: USE_OPENCV
#ifdef USE_OPENCV
  // Need a better conversion between YUYV to RGB!
  void toIplImage(IplImage *im);
  void toGrayScaleIplImage(IplImage *im);
  void toGrayScaleMat(cv::Mat& im);
  void toMat (cv::Mat& im);
  RGB at(const IplImage& m, int x, int y);
#endif

  void StopCam();

  int minBrightness();
  int maxBrightness();
  int defaultBrightness();
  int minContrast();
  int maxContrast();
  int defaultContrast();
  int minSaturation();
  int maxSaturation();
  int defaultSaturation();
  int minHue();
  int maxHue();
  int defaultHue();
  bool isHueAuto();
  int minSharpness();
  int maxSharpness();
  int defaultSharpness();

  int setBrightness(int v);
  int setContrast(int v);
  int setSaturation(int v);
  int setHue(int v);
  int setHueAuto(bool v);
  int setSharpness(int v);

  void setToImage(myImage *M) {
	  M->h = height;
	  M->w = width;
	  M->data = data;
	  M->nc = channels;
  }

  RGB at(int x, int y) const {
	  unsigned char *val;
	  val = (unsigned char *)malloc(sizeof(unsigned char)*3);	// alokasi memori
	  val = readAt(x, y, COLOR_RGB);
	  RGB out;
	  out.r = val[0];
	  out.g = val[1];
	  out.b = val[2];
	  //delete val;
	  free(val);
	  return out;
  }

  /*******************************************************\
   * Split image based on it's channel
  \*******************************************************/
  myImage* split() {
	  myImage *result;

	  result = (myImage *)malloc(sizeof(myImage)*channels);
	  for(int l = 0; l < channels; ++l) {
		  result[l].h = height;
		  result[l].w = width;
		  result[l].nc = 1;
		  // TODO: add splitting operation
		  result[l].data = (unsigned char *)malloc(result->w*result->h);
	  }
	  return result;
  }

};

#endif
