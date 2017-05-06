#ifndef _BMP_H
#define _BMP_H

#include <stdint.h>

typedef struct __attribute__((packed)) BMPHeader  {             // Total: 54 bytes
  uint16_t  type;             // Magic identifier: 0x4d42
  uint32_t  size;             // File size in bytes
  uint16_t  reserved1;        // Not used
  uint16_t  reserved2;        // Not used
  uint32_t  offset;           // Offset to image data in bytes from beginning of file (54 bytes)
  uint32_t  dib_header_size;  // DIB Header size in bytes (40 bytes)
  int32_t   width_px;         // Width of the image
  int32_t   height_px;        // Height of image
  uint16_t  num_planes;       // Number of color planes
  uint16_t  bits_per_pixel;   // Bits per pixel
  uint32_t  compression;      // Compression type
  uint32_t  image_size_bytes; // Image size in bytes
  int32_t   x_resolution_ppm; // Pixels per meter
  int32_t   y_resolution_ppm; // Pixels per meter
  uint32_t  num_colors;       // Number of colors  
  uint32_t  important_colors; // Important colors 
} BMPHeader;

typedef struct __attribute__((packed)) Pixel {
	uint8_t blue;
	uint8_t green;
	uint8_t red;
	uint8_t alpha;
} Pixel;

typedef struct BMPImage {
	BMPHeader header;
	int norm_height; //normalized height
	Pixel * * pixels;
} BMPImage;

BMPImage * readImage(FILE * fp);

void freeImage(BMPImage * image);

void writeImage(BMPImage * image, FILE * fp);

#endif
