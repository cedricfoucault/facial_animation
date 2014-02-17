#define PNG_BYTES_TO_CHECK 4
// This stores png data
struct tImagePNG
{
	int bytesPerPixel;
	int sizeX;
	int sizeY;
	unsigned char *data;
};

typedef struct tImagePNG   *PtImagePNG;

void loadPngTex(char * fname , struct tImagePNG * pImagePNG , int *hasAlpha );
void ReverseImage( int width , int height , unsigned char *data , int bytesPerPixel );
void CheckImageDimensions( int sizeX , int sizeY , char * strFileName);
void CreateTexturePNG(unsigned int *textureId, char * strFileName);
int writepng(char *filename, char *description,
	     int x, int y, int width, int height);
