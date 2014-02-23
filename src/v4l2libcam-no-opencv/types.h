/*
 * types.h
 *
 *  Created on: Feb 2, 2014
 *      Author: lynx
 *  @Brief:
 *  	// global variable used inside the library
 */

#ifndef TYPES_H_
#define TYPES_H_

#include <vector>

using namespace std;
typedef struct {
	unsigned char r,g,b;
} Vec;
typedef vector<vector<Vec> > frame;

// image container should use this typedef
typedef unsigned char** image;

typedef struct {
	int x,y;
} point;

#endif /* TYPES_H_ */
