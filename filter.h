#ifndef FILTER_H_
#define FILTER_H_

#include "bmp.h"

typedef struct BoxFilter {
	int filter[3][3];
	float norm;
} BoxFilter;

//Apply blur box filter to input image, return pointer to blurred image
BMPImage * blur(BMPImage * image);

//EXTRA CREDIT
//Apply arbitrary 3x3 box filter to input image, return pointer
BMPImage * apply(BMPImage * image, BoxFilter f);

//Read in a box filter spec from the given input file
BoxFilter * readFilter(FILE * infile);

// USER-DEFINED FUNTIONS
// retuns the final new pixel value
int newPix(BMPImage * newImg, int x, int y, char color, BoxFilter f);

// returns the UN-normalized mid-pixel value
int check(BMPImage * image, int x, int y, char color);

// makes the value between the range of 0-255
int overFlow(int midPix);


#endif
