This assignment asks you to write a simple image editing program, which will read in a bitmap file and write out a new bitmap that is a blurred version of the original. Think of this as similar to applying a blur filter in Photoshop.

> In addition to the background material discussed below, you may find the 
> following pieces of background material useful:
> 1. [Wikipedia's article on the BMP file format](https://en.wikipedia.org/wiki/BMP_file_format)
> 2. [Discussion of several types of box filters](http://tech-algorithm.com/articles/boxfiltering/)

Learning Goals
==============

In this assignment you will learn

* Reading/writing *binary* files
* Bitmaps
* Basic image processing

Background
==========

Reading and Writing Binary Files
--------------------------------

Except for
[PA01](https://engineering.purdue.edu/~milind/ece264/2017spring/assignments/pa01/),
where you didn't have to write the input/output routines, we have only dealt
with reading and writing ASCII input files: files where the data was stored as
a string of characters. `fscanf` and `fprintf` are the tools we use to read
and write data in these files. But what happens if the files we want to read
don't contain ASCII characters, but instead contain *raw binary data*? Here,
the tools we will use are `fread` and `fwrite`.

In a binary file, a number won't be represented as its ASCII character
representation, but instead as the raw sequence of bits that represents that
number. The number `17` takes up two characters in an ASCII file, but we can
represent the number `17` in a single byte in a binary file, with the raw
bits: `00010001`. If we want to read in the ASCII file containing the number 17, we would use the following code:

```
int x;
fscanf(ascii_in, "%d", &x);
```

Which would read in two characters, and perform a conversion to the number `17`. To read the raw binary data, we would do:

```
uint8_t x;
fread(&x, 1, 1, bin_in);
```

Which reads in 1 1-byte chunk of data into the variable `x` (`uint8_t` is a type that means "unsigned integer that takes up 8 bits").

Similarly, to write `27` to an ASCII file, we would use the following code:

```
uint8_t x = 27;
fprintf(ascii_out, "%d", x);
```

Which would write two characters to the output file. To write the raw data, we would do:

```
uint8_t x = 27;
fwrite(&x, 1, 1, bin_out);
```

Which would write 1 byte (with the value `00011011`) to the output file.

The signatures of `fread` and `fwrite` are:

* `size_t fread(void * ptr, size_t size, size_t nmemb, FILE * stream)`. This reads in data from `stream`. It reads in `nmemb` elements, each of size `size` (in bytes), and stores them in the location pointed to by `ptr`. It returns the total number of *elements* read (so if you multiply the return value by `size`, you get the total number of bytes read).

  A common use of `fread` is to read data from a file into an array. So, for example, if 200 elements of a structure `MyStruct` are stored in binary format in a file, you could read them using `fread(arr, sizeof(MyStruct), 200, infile);`
* `size_t fwrite(void * ptr, size_t size, size_t nmemb, FILE * stream)`. This *writes* data to `stream`. It writes `nmemb` elements, each of size `size` (in bytes) that are stored in the memory pointed to by `ptr`. It returns the total number of elements written.

  To write an array of data out to a file, you could call: `fwrite(arr, sizeof(MyStruct), 200, outfile);`

Bitmaps
-------

The bitmap (BMP) file format is one of the simplest image formats. It stores
image data pixel-by-pixel. For a given pixel, a bitmap stores 4 values
(`channels`): the red channel, the blue channel, the green channel, and the
alpha channel. Red, blue, and green are self explanatory: they determine the
intensity of the red/blue/green values of a given pixel on your screen. These
numbers can range from 0 (the channel doesn't exist in the pixel) to 255
(maximum intensity). The RGB values combine in different ways to produce
colors:

* (255, 255, 255): Maximum intensity on all channels is white
* (0, 0, 0): Zero intensity on all channels is black
* (255, 0, 0): Pure red
* (0, 255, 0): Pure green
* (0, 0, 255): Pure blue
* (255, 255, 0): Yellow
* (255, 0, 255): Magenta
* (0, 255, 255): Cyan

The alpha determines the *transparency* of a pixel (0 means transparent, 255
means opaque). Because each pixel contains values for each of these four
channels, a BMP uses a single *word* (4 bytes) to store the color of each
pixel. (Note the advantage of a binary format here! If we stored the ASCII representations of each pixel's channels, we might need up to 12 bytes to store the values of a single pixel.)

> Note that the alpha channel is not really used in bitmaps (and many bitmaps 
> only ever use the R, G and B channels), but our bitmaps are encoded as 
> 32-bit BMPs, so they use an alpha channel that is always set to 255.

Bitmaps store the contents of an image as a big array of (32-bit) pixels: Each row of pixels is stored contiguously in the file. The rows of pixels can either be stored from top to bottom or from bottom to top (see below). So how do we know how many rows of pixels we have? Or how many pixels are in each row? We use the bitmap *header*. A BMP includes information at the beginning of the file describing the rest of the file. This header is also stored in a binary format, and takes up 54 bytes. The details of what is in the header is included in `bmp.h`:

```
typedef struct __attribute__((packed)) BMPHeader  {
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
```

The order of the fields in the structure corresponds to the order of the data in the file's header. (The `__attribute__((packed))` annotation lets the compiler know not to put any padding between the fields of the structure). To read in this header from the file, we could do:

```
BMPHeader h;
fread(&h, sizeof(image->header), 1, fp);
```

> Note that the way we are reading in the header *is not strictly portable*.
> The BMP file header is "packed": the fields of the header are right next to
> each other in the file, with no padding. Furthermore, the BMP file format
> is *little endian*: all multi-byte fields are stored with the 
> least-significant byte first (see the [PA02](https://engineering.purdue.edu/~milind/ece264/2017spring/assignments/pa01/) 
> discussion on endianness). That means that `fread`ing in the entire struct
> at once requires that the structure layout in the program is packed (which 
> not all compilers will do) and that the machine we are running the program
> on is little-endian (which, luckily, all x86 machines are).
>
> To write this in a more portable way would require quite a lot more code.
> The code isn't difficult, but it is tedious to write.


There is a lot of data here, but the important fields are:

* `size`: how big the file is in bytes. This should be 54 + width * height.
* `width_px`: how many pixels wide the image is. This is how many pixels are
  on each "row" of the image.
* `height_px`: how many pixels tall the image is. This is how many rows there
  are of pixels. **Note something important**. This number can be negative. If
  the number is negative, it *does not* mean that the image has a negative 
  height. It means that the rows are stored top to bottom in the file. A
  positive number means they are stored bottom to top.
* `bits_per_pixel`: for our images, this will be 32.

Once you have read in the header, you know how many pixels are in the image,
and can read them into a two-dimensional array (recall 2D arrays from
[PA07](https://engineering.purdue.edu/~milind/ece264/2017spring/assignments/pa07/)). The structure to hold an overall image is in `bmp.h`:

```
typedef struct BMPImage {
	BMPHeader header;
	int norm_height; //normalized height
	Pixel * * pixels;
} BMPImage;
```

(Note that `norm_height` holds the height of the image in pixels. We store 
this separately from `height_px` because the latter might be negative. 
`norm_height` should always be positive.)

The `Pixel` data structure is:

```
typedef struct __attribute__((packed)) Pixel {
	uint8_t blue;
	uint8_t green;
	uint8_t red;
	uint8_t alpha;
} Pixel;
```

Note that the order the fields are defined in this structure matches the order
they exist in the file (so if a Pixel takes up 4 bytes in the file, the first
byte is the value of the blue channel, the second is the value of the green,
etc.).

> Note that the order of the channels here reads out as "BGRA", which seems
> reversed from the order we're used to (alpha + RGB). This is because BMPs
> are a little-endian format: what we would normally write as the first byte
> is actually the last byte in the 4-byte word.

Basic image processing
----------------------

One of the most basic image manipulation tasks is *filtering* an image: applying a transformation to it to blur the image, sharpen the image, etc. It turns out that there is a general framework for manipulating images that can blur or sharpen images, or detect edges, using the same strategy. The idea is performing a *convolution* using a *box filter*. To understand what this means, let us think about how we could blur an image.

Let's consider our image to be a large grid of pixels. To blur an image, one thing we could do is "smear" the colors through the image: mix the colors of each pixel with the colors of its neighbors. We can do this in a general way using a box filter, which looks like a 3-by-3 grid itself:

```
1 1 1
1 1 1
1 1 1
```

If we overlay the center of the box filter on top of a single target pixel,
then the surrounding pixels sit "on top" of the target's neighboring pixels.
To *apply* a box filter, we take each channel, multiply the values in each
cell of the box filter by the channel value of the pixel they sit on top of,
and add them together (so in this case, for example, we multiply each of the
red channels by 1 and add them together -- effectively adding up the red
channels of the target pixel and all eight of its neighbors -- and repeat for
each of the other three channels). We then *normalize* the channel values by
dividing through by a normalization factor specified for the particular
filter. In this case, the normalization factor is 9.0. This gives us the new
channel values for the target pixel. 

For example, if the red channel values for the target pixel and its neighbors are:

```
201 191 210
293 100 102
200  75  50
```

The output red channel for the target pixel will be 158.

If we do this for every channel and every pixel in the image, we can output
the new values to a *new image* to create a blurred version of the image.

> The filter example we have here performs a *linear blur*. But other box 
> filters produce other effects. For example, the filter
>
> `-1 -1 -1`
>
> `-1 17 -1`
>
> `-1 -1 -1`
>
> with a normalization factor of 9.0 will *sharpen* the image instead of blur
> it. The filter
>
> `-1 0 1`
>
> `-1 0 1`
>
> `-1 0 1`
>
> with a normalization factor of 1.0 will perform *edge detection* and produce
> an output image with only the edges left. 

A couple of details:

1. What do we do at the boundaries of the image? If the target pixel is at the
   boundary of the image, it does not have 8 neighbors. There are many
   approaches to solving this problem. In this assignment, we will use a
   strategy called *extension*. We will assume the boundary pixels are 
   "copied" to create a virtual boundary one pixel around the image, and use
   those values when we need the necessary neighbors.
2. What to do if the output channel values are > 255 or < 0? In these cases,
   we will "clip" the channel value: if it is > 255, set the output value to
   255; if it < 0, set the output value to 0. *Make sure you only clip the channel values after normalization*, not before.

What you have to do
===================

Your job is to:

1. Write a routine to read BMP image files. This means allocating a `BMPImage`
   structure as described in `bmp.h` and allocating and initializing the 
   pixels.
2. Write a routine to generate a *new* `BMPImage` structure that contains a
   blurred version of the original input image. Use the 3x3 linear blur box
   filter and approach described above.
3. Write a routine to write that new BMP image to an output file. This means 
   that you need to get the header format correct (hint: you should be able to
   reuse a lot of data in the input image's header).
4. Write a routine to free image data structures. This routine should fully 
   free the image data structure (i.e., we will not later call `free` on the 
   pointer you pass in).

We have provided `bmp.h`, which declares the necessary header, image, and
pixel structures, as well as three methods, `readImage`, `writeImage`, and `freeImage`. Your job is to fill in those method definitions in `bmp.c`.

We have also provided `filter.h`, which declares a method called `blur`. Your
job is to fill in that method definition in `filter.h`.

If you need/want to define additional methods in your files, feel free to. *Do not create any additional files.*

Testing your code
-----------------

You will need to write your own Makefile and test code.

The easiest way to test whether your image read and write functions work
correctly is to use them to duplicate a bitmap.

We have also provided a validator program (`validate.c`) among the starter
files that you can use to help check if your filter works correctly. This test
program *requires* that you have a working `readImage` and `writeImage`
function. Note that because you may perform your filter calculations in a
slightly different order than we do, the validator has a built-in error
tolerance.

You can download a [test image](https://engineering.purdue.edu/~milind/ece264/2017spring/assignments/pa13/test.bmp) and a sample [output image](https://engineering.purdue.edu/~milind/ece264/2017spring/assignments/pa13/outBlur.bmp) that shows the result of applying the blur filter to the input image.

Partial Credit
--------------

We will test your `readImage`, `writeImage` and `blur` functions separately (`freeImage` will be tested along with `readImage`),
and assign partial credit if only some work: `readImage` and `writeImage` are
worth 25% of your grade each, and `blur` is worth 50%.

*Do not remove the #ifndef and #define directives from the provided files!*

Extra credit
------------

This assignment has an opportunity for extra credit (a 25% bonus on your
grade). Define the functions `readFilter` (which reads a box filter definition
from a file and returns a pointer to a `BoxFilter` structure) and `apply`
(which applies a specified box filter to an input image).

The format of a filter definition is 3 rows defining the filter (each row
contains 3 integer values) followed by 1 row specifying the normalization
factor (a floating point value). `sharpen.filter`, provided in the starter
files, is an example filter definition.

[outSharp.bmp](https://engineering.purdue.edu/~milind/ece264/2017spring/assignments/pa13/outSharp.bmp) is the output of running the sharpening filter on the test image.

What to submit
--------------

1. Submit your `bmp.c` and, if you added any new methods to it, `bmp.h`.
2. Submit your `filter.c` and, if you added any new methods to it, `filter.h`.

*Do not submit any other files.*