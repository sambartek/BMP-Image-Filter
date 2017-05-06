#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "bmp.h"
#include "filter.h"

int main(int argc, char ** argv) {
  if (argc != 4) {
    fprintf(stderr, "Usage ./validate actual.bmp expected.bmp\n");
    return EXIT_FAILURE;
  }
  // the original file
  FILE * actualFile = fopen(argv[1], "rb");
  if (actualFile == NULL) {
    printf("File could not be opened.\n");
    return EXIT_FAILURE;
  }

  //the new copy of the original file
  BMPImage * NewImage = readImage(actualFile);
  if (NewImage == NULL) {
    printf("Image could not be read.\n");
    freeImage(NewImage);
    fclose(actualFile);
    return EXIT_FAILURE;
  }
  // new file for write the new image to
  FILE * newFile = fopen(argv[2], "wb");
  if (newFile == NULL) {
    printf("New file could not be opened.\n");
    return EXIT_FAILURE;
  }

  // test for blur
  /*BMPImage * blurImg = blur(NewImage);
  writeImage(blurImg, newFile);
  freeImage(blurImg);
  */
  // test for apply
  FILE * filterFile = fopen(argv[3], "r");
  if (filterFile == NULL) {
    printf("Filter file could not be opened.\n");
    return EXIT_FAILURE;
  }
  BoxFilter * filter = readFilter(filterFile);
  BoxFilter t_filter = *filter;
  BMPImage * filterImg = apply(NewImage, t_filter);
  writeImage(filterImg, newFile);
  free(filter);

  // close and free stuff
  freeImage(NewImage);
  freeImage(filterImg);
  fclose(newFile);
  fclose(actualFile);
  return EXIT_SUCCESS;
}
