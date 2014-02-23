/*
 * ipl.cpp
 * function definition for ipl.h
 *      Author: lynx
 */


#include <string.h>
#include "ipl.h"
#include "libcam.h"

typedef struct
{
	int width;
	int height;
	int maxgrey;
	unsigned char* data;
	int flag;
} MyImage ;

char* strrev(char* str);

void itochar(int x, char *szBuff, int radix);

int writePgm(char *fileName, MyImage *image)
{
	char parameters_str[5];
	int i;
	const char *format = "P5";
	if (image->flag == 0)
	{
		return -1;
	}
	FILE *fp = fopen(fileName, "w");
	if (!fp)
	{
		printf("Unable to open file %s\n", fileName);
		return -1;
	}
	fputs(format, fp);
	fputc('\n', fp);

	itochar(image->width, parameters_str, 10);
	fputs(parameters_str, fp);
	parameters_str[0] = 0;
	fputc(' ', fp);

	itochar(image->height, parameters_str, 10);
	fputs(parameters_str, fp);
	parameters_str[0] = 0;
	fputc('\n', fp);

	itochar(image->maxgrey, parameters_str, 10);
	fputs(parameters_str, fp);
	fputc('\n', fp);

	for (i = 0; i < (image->width * image->height); i++)
	{
		fputc(image->data[i], fp);
	}
	fclose(fp);
	return 0;
}

void itochar(int x, char *szBuff, int radix) {
	int i = 0 , n , xx;
	n = x;
	while(n > 0) {
		xx = n%radix;
		n = n/radix;
		szBuff[i++] = '0' + xx;
	}
	szBuff[i] = '\0';
	strrev(szBuff);
}

char* strrev(char* str)
{
	char *p1, *p2;
	if (!str || !*str)
		return str;
	for (p1 = str, p2 = str + strlen(str) - 1; p2 > p1; ++p1, --p2)
	{
		*p1 ^= *p2;
		*p2 ^= *p1;
		*p1 ^= *p2;
	}
	return str;
}

/******************************************************\
 *	NOTE:				PIXEL
\******************************************************/
class PIXEL {
public:
	PIXEL(int nChannels) {
		nc = nChannels;
	}

private:
	int nc;
};

/******************************************************\
 *	NOTE:				IMAGING
\******************************************************/

IMAGING::IMAGING(int width, int height, unsigned char *data, int nChannels) {
		w = width; h = height;	// dimension
		p = data;

		if(nChannels == 4) /* YUYV considered */ {
			rgb_data = GenerateRGB();
			nc = 3;
		} else if(nChannels == 1) {	// single channel
			nc = nChannels;
		}
		size.w = w;
		size.h = h;
		channels = nc;
}

IMAGING::IMAGING(myImage *I)  {
	w = I->w;
	h = I->h;
	nc = I->nc;
	p = I->data;
}

/*template<typename _datatype>
_datatype* IMAGING::at(int x, int y) const {
	_datatype *d;
	d = (_datatype *)malloc(sizeof(_datatype)*nc);
	int i=( (y*w/2) + x)*nc;
	for(int idx = 0; idx < nc; ++idx) {
		d[idx] = p[i+idx];
	}
	return d;
}*/

RGB IMAGING::GetRGB(int x, int y) const {
	RGB o;
	int i=( (y*w/2) + x)*nc;
	o.r = (p[i]);
	o.g = (p[i+1]);
	o.b = (p[i+2]);
	return o;
}

/*
 * output data format is {[RGB],[RGB]...}
 * TODO: need better algorithm!
 * 	// tough the data format is for my own purpose, but we need to increase performance
 *	// in order to do so, i need pre-formatted data output!
 */
unsigned char* IMAGING::GenerateRGB() {
		// allocate memory!
		rgb_data = (unsigned char*)malloc(sizeof(unsigned char)*w*h*3);	// 3 is the number of channels
		for(int x = 0; x < w/2; x++) {
			for(int y = 0; y < h; y++) {
				int y0, y1, u, v;	// buffer
				int i = ( (y*w/2) + x)*4;	// data index
				y0 = p[i]; u = p[i+1]; y1 = p[i+2]; v = p[i+3];
				int r,g,b;
				r = y0 + (1.370705 * (v-128));
				g = y0 - (0.698001 * (v-128)) - (0.337633 * (u-128));
				b = y0 + (1.732446 * (u-128));

				if(r > 255) r = 255;
				if(g > 255) g = 255;
				if(b > 255) b = 255;
				if(r < 0) r = 0;
				if(g < 0) g = 0;
				if(b < 0) b = 0;

				i = (y*w + 2*x)*3;
				rgb_data[i] = (unsigned char)(r);
				rgb_data[i+1] = (unsigned char)(g);
				rgb_data[i+2] = (unsigned char)(b);

				r = y1 + (1.370705 * (v-128));
				g = y1 - (0.698001 * (v-128)) - (0.337633 * (u-128));
				b = y1 + (1.732446 * (u-128));

				if(r > 255) r = 255;
				if(g > 255) g = 255;
				if(b > 255) b = 255;
				if(r < 0) r = 0;
				if(g < 0) g = 0;
				if(b < 0) b = 0;

				// fill next pixel
				rgb_data[i+3] = (unsigned char)(r);
				rgb_data[i+4] = (unsigned char)(g);
				rgb_data[i+5] = (unsigned char)(b);
			}
		}
		return rgb_data;
	}

// clear memory
void IMAGING::Clear(){
	free(p);
}

IMAGING::~IMAGING() {
//	Clear();	// if we define this, then it will terminate. I don't know why.
}


