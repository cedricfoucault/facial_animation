/*****************************************************************************
File: texpng.cpp

Virtual Humans
Master in Computer Science
Christian Jacquemin, University Paris 11

This file is provided without support, instruction, or implied
warranty of any kind.  University Paris 11 makes no guarantee of its
fitness for a particular purpose and is not liable under any
circumstances for any damages or loss whatsoever arising from the use
or inability to use this file or items derived from it.
******************************************************************************/
#include <GL/gl.h>
#include <GL/glu.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <png.h>
#include "texpng.h"
#include "ctype.h"

#define png_infopp_NULL (png_infopp)NULL
#define int_p_NULL (int*)NULL

/* Pixels in this bitmap structure are stored as BGR. */
typedef struct _RGBPixel {
    unsigned char blue;
    unsigned char green;
    unsigned char red;
} RGBPixel;

/* Structure for containing decompressed bitmaps. */
typedef struct _RGBBitmap {
    RGBPixel *pixels;
    size_t width;
    size_t height;
    size_t bytewidth;
    unsigned char bytes_per_pixel;
} RGBBitmap;

///////////////////////////////// LOAD PNG \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This loads the PNG file and returns its data in a tImagePNG struct
/////
///////////////////////////////// LOAD PNG \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

// This is the interesting function here. It sets up a PNG image as a texture.
void loadPngTex(char * fname , struct tImagePNG * pImagePNG , int *hasAlpha )
{
  FILE      *fp;
  
  // The header of the file will be saved in here
  char buf[PNG_BYTES_TO_CHECK];
  
  // Open the file and check correct opening
  /* Open the prospective PNG file. */
  if ((fp = fopen(fname, "rb")) == NULL) {
    printf( "Error: Could not open the texture file [%s]!" , fname );
    exit( 0 );
  }
  
  // Read the PNG header, which is 8 bytes long.
  /* Read in some of the signature bytes */
  if (fread(buf, 1, PNG_BYTES_TO_CHECK, fp) != PNG_BYTES_TO_CHECK) {
    printf( "Error: Incorrect PNG texture file [%s]!" , fname ); 
    exit( 0 );
  }

  // Check whether the file is a PNG file
  // png_sig_cmp() checks the given PNG header and returns 0 if it could
  // be the start of a PNG file. We can use 8 bytes at max for
  // this comparison.
  if(png_sig_cmp((png_byte*)buf, (png_size_t)0, PNG_BYTES_TO_CHECK) != 0) {
    fclose( fp );
    printf( "Error: Not a PNG file [%s]!" , fname );
    exit( 0 );
  }
  
  // Create / initialize the png_struct
  // The png_struct isn't directly accessed by the user (us).
  // We will later create a png_info from this to get access to the
  // PNG's infos.
  // The three 0's in the arg list could be pointers to user defined error
  // handling functions. 0 means we don't want to specify them, but use
  // libPNG's default error handling instead.
  png_infop info_ptr;
  png_structp png_ptr
    = png_create_read_struct(PNG_LIBPNG_VER_STRING,
			     NULL , NULL , NULL );
  if(!png_ptr) {
    fclose( fp );
    printf( "Error: Couldn't create PNG read structure [%s]!" , fname );
    exit( 0 );
  }

  // Create / initialize the png_info
  // The png_info grants the user access to the PNG infos.
  info_ptr = png_create_info_struct(png_ptr);
  if(!info_ptr) {
    // We need to destroy the read_struct
    png_destroy_read_struct(&png_ptr, NULL, NULL);
    fclose( fp );
    printf( "Error: Couldn't create PNG info structure [%s]!" , fname );
    exit( 0 );
  }
  
  // Setup error handler
  // This sets the point libPNG jumps back to is an error occurs.
  if (setjmp(png_jmpbuf(png_ptr))) {
    /* Free all of the memory associated with the png_ptr and info_ptr */
    png_destroy_read_struct(&png_ptr, &info_ptr, png_infopp_NULL);
    fclose(fp);
    /* If we get here, we had a problem reading the file */
    printf( "Error: Couldn't setup PNG error handler [%s]!" , fname );
    exit( 0 );
  }
  
  /* Set up the input control if you are using standard C streams */
  png_init_io(png_ptr, fp);
  
  // This tells libPNG that we have already read 8 bytes from the start
  // of the file (for the header check above).
  png_set_sig_bytes(png_ptr, PNG_BYTES_TO_CHECK);

  // This reads the PNG file into the read and info structs
  png_read_png(png_ptr, info_ptr, PNG_TRANSFORM_IDENTITY, NULL);

  // Get some basic infos about the image
  pImagePNG->sizeX = png_get_image_width(png_ptr, info_ptr); // width in px
  pImagePNG->sizeY = png_get_image_height(png_ptr, info_ptr); // height in px
  int bit_depth = png_get_bit_depth(png_ptr, info_ptr); // bits per pixel

  // Color type - we'll handle RGB and RGBA (with Alpha)
  int cType = png_get_color_type(png_ptr, info_ptr);

  // We now calculate the *bytes* per pixel from the color type and the
  // bits per pixel.
  if((cType == PNG_COLOR_TYPE_RGB) && (bit_depth == 8)) {
    pImagePNG->bytesPerPixel = 3;
     *hasAlpha = 0;
  }
  else if((cType == PNG_COLOR_TYPE_RGB_ALPHA) && (bit_depth == 8)) {
    pImagePNG->bytesPerPixel = 4;
    *hasAlpha = 1;
  }
  else {
    /* clean up after the read, and free any memory allocated - REQUIRED */
    png_destroy_read_struct(&png_ptr, &info_ptr, png_infopp_NULL);
    fclose( fp );
    printf( "Error: PNG image [%s] type is unsupported!" , fname );
    exit( 0 );
  }
  
  // Returns a pointer to the array of array of png_bytes that holds the
  // image data.
  png_byte **imageData = png_get_rows(png_ptr, info_ptr);

  // This will hold our image 
  unsigned int rowbytes = png_get_rowbytes(png_ptr, info_ptr);
  pImagePNG->data = (GLubyte*)malloc(pImagePNG->sizeY * rowbytes * sizeof(GLubyte));
  
  int i,j;
  for( i = 0 ; i < pImagePNG->sizeY ; i++ ) {
    memcpy(&pImagePNG->data[(pImagePNG->sizeY - i - 1) * rowbytes],
	   imageData[i], rowbytes);
  }

  // Free the memory we used - we don't need it anymore
  /* clean up after the read, and free any memory allocated - REQUIRED */
  png_destroy_read_struct(&png_ptr, &info_ptr, png_infopp_NULL);
  fclose( fp );
}

///////////////////////////// CREATE TEXTURE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This creates a texture in OpenGL that we can use as a texture map
/////
///////////////////////////// CREATE TEXTURE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void ReverseImage( int width , int height , unsigned char *data , int bytesPerPixel ) {
  unsigned char *dataaux = (unsigned char*)malloc(width * height * bytesPerPixel * sizeof(unsigned char));
  int indrow;
  for( indrow = 0 ; indrow < height ; indrow++ ) {
    memcpy(&dataaux[indrow * width * bytesPerPixel],
	   &data[(height - indrow - 1) * width * bytesPerPixel], width * bytesPerPixel);
  }
  for( indrow = 0 ; indrow < height ; indrow++ ) {
    memcpy(&data[indrow * width * bytesPerPixel],
	   &dataaux[indrow * width * bytesPerPixel], width * bytesPerPixel);
  }
  free(dataaux);
}

// The image dimensions must be powers of 2.
void CheckImageDimensions( int sizeX , int sizeY , char * strFileName) {  
  // Check the image width
  int comp = 2;
  int isValid = 0;
  int i;
  for( i = 0; i < 10; i++) {
    comp <<= 1;
    if(sizeX == comp)
      isValid = 1;
  }
  if(!isValid) {
    printf( "Error: The image width is not a power of 2 [%s]!" , strFileName ); 
    exit( 0 );
  }  
  
  // Check the image height
  comp = 2;
  isValid = 0;
  for( i = 0; i < 10; i++) {
    comp <<= 1;
    if(sizeY == comp)
      isValid = 1;
  }
  if(!isValid) {
    printf( "Error: The image height is not a power of 2 [%s]!" , strFileName );
    exit( 0 );
  }  
}

///////////////////////////////// CREATE TEXTURE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This creates a texture in OpenGL that we can use as a texture map
/////
///////////////////////////////// CREATE TEXTURE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void CreateTexturePNG(unsigned int *textureId, char * strFileName)
{
  if(!strFileName)			
    // Return from the function if no file name was passed in
    return;
  
  
  printf( "Loading [%s]!\n" , strFileName ); 

  // Load the image and store the data
  struct tImagePNG imagePNG;
  int hasAlpha = 0;
  loadPngTex(strFileName , &imagePNG , &hasAlpha );  
	
  // Generate a texture with the associative texture ID stored in the array
  glGenTextures(1, textureId);

  // Bind the texture to the texture arrays index and init the texture
  glBindTexture(GL_TEXTURE_2D, *textureId);

  // Build Mipmaps (builds different versions of the picture for distances - looks better)
  if( hasAlpha ) {
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,  
		    GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, 
		    GL_LINEAR);
    gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGBA8,
		      imagePNG.sizeX, imagePNG.sizeY,  
		      GL_RGBA, 
		      GL_UNSIGNED_BYTE, (const void *)imagePNG.data); 
  }
  else {
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,  
		    GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, 
		    GL_LINEAR);
    gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGB8,
		      imagePNG.sizeX, imagePNG.sizeY,  
		      GL_RGB, 
		      GL_UNSIGNED_BYTE, (const void *)imagePNG.data); 
  }

  // Now we need to free the image data that we loaded since OpenGL stored it as a texture

  if (imagePNG.data) 	        // If there is texture data
    {
      free(imagePNG.data);	// Free the texture data, we don't need it anymore
    }
}

//////////////////////////////////////////////////////////////////////
// SAVE IMAGE
//////////////////////////////////////////////////////////////////////

/* Attempts to save PNG to file; returns 0 on success, non-zero on error. */
int writepng(char *filename, char *description,
	     int x, int y, int width, int height) {
  RGBPixel *image;
  int bytes_per_pixel = 3;

  FILE *fp;
  png_structp png_ptr = NULL;
  png_infop info_ptr = NULL;
  png_uint_32 bytes_per_row;
  png_byte **row_pointers = NULL;

  fp = fopen(filename, "wb");
  if (fp == NULL) {
    return 1;
  }
  image = (RGBPixel *) malloc(width * height * sizeof(struct _RGBPixel));

  /* OpenGL's default 4 byte pack alignment would leave extra bytes at the
     end of each image row so that each full row contained a number of bytes
     divisible by 4.  Ie, an RGB row with 3 pixels and 8-bit componets would
     be laid out like "RGBRGBRGBxxx" where the last three "xxx" bytes exist
     just to pad the row out to 12 bytes (12 is divisible by 4). To make sure
     the rows are packed as tight as possible (no row padding), set the pack
     alignment to 1. */
  glPixelStorei(GL_PACK_ALIGNMENT, 1);
  glReadPixels(x, y, width, height, GL_RGB, GL_UNSIGNED_BYTE, (void *)image);

  /* Initialize the write struct. */
  png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
  if (png_ptr == NULL) {
    fclose(fp);
    return -1;
  }

  /* Initialize the info struct. */
  info_ptr = png_create_info_struct(png_ptr);
  if (info_ptr == NULL) {
    png_destroy_write_struct(&png_ptr, NULL);
    fclose(fp);
    return 1;
  }

  /* Set up error handling. */
  if (setjmp(png_jmpbuf(png_ptr))) {
    png_destroy_write_struct(&png_ptr, &info_ptr);
    fclose(fp);
    return 1;
  }

  /* Set image attributes. */
  png_set_IHDR(png_ptr,
	       info_ptr,
	       width,
	       height,
	       8,
	       PNG_COLOR_TYPE_RGB,
	       PNG_INTERLACE_NONE,
	       PNG_COMPRESSION_TYPE_DEFAULT,
	       PNG_FILTER_TYPE_DEFAULT);
  
  /* Initialize rows of PNG. */
  bytes_per_row = width * bytes_per_pixel;
  row_pointers = (png_byte **)png_malloc(png_ptr, height * sizeof(png_byte *));
  unsigned char *pixels = (unsigned char *)image;
  for (int j = height - 1; j >= 0 ; --j) {
    unsigned char *row = (unsigned char *)png_malloc(png_ptr, sizeof(unsigned char) * bytes_per_row);
    row_pointers[j] = (png_byte *)row;
    memcpy(row_pointers[j], pixels, bytes_per_pixel * width);
    pixels += bytes_per_pixel * width;
    // for (int i = 0; i < width; ++i) {
    //   RGBPixel color = *(image + (j * bytes_per_row) + (i * bytes_per_pixel));
    //   *row++ = color.red;
    //   *row++ = color.green;
    //   *row++ = color.blue;
    // }
  }
  
  /* Actually write the image data. */
  png_init_io(png_ptr, fp);
  png_set_rows(png_ptr, info_ptr, row_pointers);
  png_write_png(png_ptr, info_ptr, PNG_TRANSFORM_IDENTITY, NULL);

  /* Cleanup. */
  for (int j = 0; j < height; j++) {
    png_free(png_ptr, row_pointers[j]);
  }
  png_free(png_ptr, row_pointers);

  /* Finish writing. */
  png_destroy_write_struct(&png_ptr, &info_ptr);
  free(image);
  fclose(fp);
  return 0;
}
