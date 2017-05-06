#include <stdlib.h>
#include <stdio.h>
#include "filter.h"

#ifndef BLUR_OFF
BMPImage * blur(BMPImage * image) {
  //FILL IN
  //makes new image to use filters on
  BMPImage * newImg = malloc(sizeof(BMPImage)); // free in main

  // copying given image to new image
  newImg->norm_height = image->norm_height;
  newImg->header = image->header;

  // creating space for new pixels data
  newImg->pixels = malloc((newImg->norm_height) * sizeof(Pixel *));
  for (int i = 0; i < newImg->norm_height; i++) {
    newImg->pixels[i] = malloc((newImg->header.width_px) * sizeof(Pixel));
  }

  // copying pixel data from old image to new
  int y;
  int x;
  for (y = 0; y < newImg->norm_height; y++) {
    for (x = 0; x < newImg->header.width_px; x++) {
      newImg->pixels[y][x] = image->pixels[y][x];
    }
  }

  // needs a filter so blur & extra credit both work
  BoxFilter f;
  for (int k = 0; k < 3; k++) {
    for (int j = 0; j < 3; j++) {
      f.filter[k][j] = 1;
      //printf("filter k: %d j: %d val: %d\n", k, j, f.filter[k][j]);
    }
  }

  f.norm = 9; // normalizing factor for blur
  //printf("f norm: %f\n",f.norm);

  // blurring the new image using blurring algorithm
  int midPixB = 0;
  int midPixG = 0;
  int midPixR = 0;
  for (y = 0; y < newImg->norm_height; y++) {
    for (x = 0; x < newImg->header.width_px; x++) {
      //sum for blue
      midPixB = newPix(newImg, x, y, 'b', f);
      //sum for green
      midPixG = newPix(newImg, x, y, 'g', f);
      //sum for red
      midPixR = newPix(newImg, x, y, 'r', f);

      // makes sure the pixels are in the range of 0-255
      midPixB = overFlow(midPixB);
      midPixG = overFlow(midPixG);
      midPixR = overFlow(midPixR);

      // assigns the values of newly calculated pixel to target pixel
      newImg->pixels[y][x].blue = midPixB;
      newImg->pixels[y][x].green = midPixG;
      newImg->pixels[y][x].red = midPixR;
    }
  }
  return newImg;
}
#endif

int newPix(BMPImage * newImg, int x, int y, char color, BoxFilter f) {
  int midPix;
  midPix = check(newImg, x+1, y+1, color) * f.filter[2][2];
  midPix += check(newImg, x-1, y+1, color) * f.filter[2][0];
  midPix += check(newImg, x, y+1, color) * f.filter[2][1];
  midPix += check(newImg, x+1, y-1, color) * f.filter[0][2];
  midPix += check(newImg, x-1, y-1, color) * f.filter[0][0];
  midPix += check(newImg, x, y-1, color) * f.filter[0][1];
  midPix += check(newImg, x-1, y, color) * f.filter[1][0];
  midPix += check(newImg, x+1, y, color) * f.filter[1][2];
  midPix += check(newImg, x, y, color) * f.filter[1][1];
  return (midPix / f.norm);
}

int check(BMPImage * image, int x, int y, char color) {
  if (x < 0) {
    x++; // shift right
  }
  if (x >= image->header.width_px) {
    x--; //shift left
  }
  if (y < 0) {
    y++; //shift down
  }
  if (y >= image->norm_height) {
    y--; //shift up
  }
  if (color == 'b') {
    return image->pixels[y][x].blue;
  }
  else if (color == 'g') {
    return image->pixels[y][x].green;
  }
  else if (color == 'r') {
    return image->pixels[y][x].red;
  }
  else {
    return image->pixels[y][x].alpha;
  }
}

int overFlow(int midpix) {
  if (midpix > 255) {
    return 255;
  }
  else if (midpix < 0 ){
    return 0;
  }
  else {
    return midpix;
  }
}

// EXTRA CREDIT START
BMPImage * apply(BMPImage * image, BoxFilter f) {
  int midPixB = 0;
  int midPixG = 0;
  int midPixR = 0;
  int midPixA = 0;
  int y;
  int x;

  BMPImage * newImg = malloc(sizeof(BMPImage)); // free in main

  // copying given image to new image
  newImg->norm_height = image->norm_height;
  newImg->header = image->header;

  // creating space for new pixels data
  newImg->pixels = malloc((newImg->norm_height) * sizeof(Pixel *));
  for (int i = 0; i < newImg->norm_height; i++) {
    newImg->pixels[i] = malloc((newImg->header.width_px) * sizeof(Pixel));
  }

  for (y = 0; y < image->norm_height; y++) {
    for (x = 0; x < image->header.width_px; x++) {
      //sum for blue
      midPixB = newPix(image, x, y, 'b', f);
      //sum for green
      midPixG = newPix(image, x, y, 'g', f);
      //sum for red
      midPixR = newPix(image, x, y, 'r', f);
      //sum for alpha
      midPixA = newPix(image, x, y, 'a', f);

      // makes sure the pixels are in the range of 0-255
      midPixB = overFlow(midPixB);
      midPixG = overFlow(midPixG);
      midPixR = overFlow(midPixR);
      midPixA = overFlow(midPixA);

      // assigns the values of newly calculated pixel to target pixel
      newImg->pixels[y][x].blue = midPixB;
      newImg->pixels[y][x].green = midPixG;
      newImg->pixels[y][x].red = midPixR;
      newImg->pixels[y][x].alpha = midPixA;
    }
  }
  return newImg;
}

BoxFilter * readFilter(FILE * infile) {
  //FILL IN
  if (infile == NULL) {
    printf("infile is NULL.\n");
    return NULL;
  }

  BoxFilter * f = malloc(sizeof(BoxFilter));

  //read in the filter
  for (int y = 0; y < 3; y++) {
    for (int x = 0; x < 3; x++) {
      if (fscanf(infile, "%d", &(f->filter[y][x])) != 1) {
        printf("Not scanning filter properly.\n");
        return NULL;
      }
    }
  }

  //read in normalizing factor
  if (fscanf(infile, "%f", &(f->norm)) != 1) {
    printf("Filter normalizing not scanning.\n");
    return NULL;
  }
  return f;
}
// EXTRA CREDIT END
