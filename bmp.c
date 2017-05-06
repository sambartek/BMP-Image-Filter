#include <stdio.h>
#include <stdlib.h>
#include "bmp.h"

#ifndef READIMAGE_OFF
BMPImage * readImage(FILE * fp) {
// FILL IN
	if (fp == NULL) {
		//printf("File pointer NULL.\n");
		return NULL;
	}

	BMPImage * newImg = malloc(sizeof(BMPImage)); // need to free from main somehow
	
  //reads in the header from the given file
	fread(&(newImg->header), sizeof(BMPHeader), 1, fp);
	
  //printf("The file contains %d bytes\n", newImg->header.size); // remove test

  // makes neg height to pos
  newImg->norm_height = abs(newImg->header.height_px); 
	
  // remove after testing
  //printf("File width: %d.\n", newImg->header.width_px);
  //printf("File height: %d.\n", newImg->norm_height); 
  
  // Allocate 2-D array for pixels in newImg
  newImg->pixels = malloc((newImg->norm_height) * sizeof(Pixel *));
  for (int i = 0; i < newImg->norm_height; i++) {
    newImg->pixels[i] = malloc((newImg->header.width_px) * sizeof(Pixel));
  }

  // read in old image pixels to new image, row by row
  // may need 1 more for-loop
  for (int y = 0; y < newImg->norm_height; y++) {
      fread(newImg->pixels[y], sizeof(Pixel), newImg->header.width_px, fp);
  }

	return newImg;
}

void freeImage(BMPImage * image) {
  // free's the rows of the image
  for (int y = 0; y < image->norm_height; y++) {
      free(image->pixels[y]);
  }
  // free's the "spine" of the pixels 
  free(image->pixels);
  free(image);
}

#endif


#ifndef WRITEIMAGE_OFF

void writeImage(BMPImage * image, FILE * fp) {
  // FILL IN
  if (fp == NULL) {
    return;
  }
  
  // writes the header to the output image
  fwrite(&(image->header), sizeof(BMPHeader), 1, fp);
  
  //writes the image by pixel row to new file 
  for (int y = 0; y < image->norm_height; y++) {
    fwrite(image->pixels[y], sizeof(Pixel), image->header.width_px, fp);
  }
  return;
}
#endif
