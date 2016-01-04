#ifndef __BITMAP_H
#define __BITMAP_H

/** @defgroup Bitmap Bitmap
 * @{
 * Functions for manipulating bitmaps
 */

typedef enum {
	ALIGN_LEFT, ALIGN_CENTER, ALIGN_RIGHT
} Alignment;

typedef struct {
	unsigned short type; // specifies the file type
	unsigned int size; // specifies the size in bytes of the bitmap file
	unsigned int reserved; // reserved; must be 0
	unsigned int offset; // specifies the offset in bytes from the bitmapfileheader to the bitmap bits
} BitmapFileHeader;

typedef struct {
	unsigned int size; // specifies the number of bytes required by the struct
	int width; // specifies width in pixels
	int height; // specifies height in pixels
	unsigned short planes; // specifies the number of color planes, must be 1
	unsigned short bits; // specifies the number of bit per pixel
	unsigned int compression; // specifies the type of compression
	unsigned int imageSize; // size of image in bytes
	int xResolution; // number of pixels per meter in x axis
	int yResolution; // number of pixels per meter in y axis
	unsigned int nColors; // number of colors used by the bitmap
	unsigned int importantColors; // number of colors that are important
} BitmapInfoHeader;

/// Represents a Bitmap
typedef struct {
	BitmapInfoHeader bitmapInfoHeader;
	unsigned char* bitmapData;
} Bitmap;

/**
 * @brief Loads a bmp image
 * @param filename Path of the image to load
 * @return Non NULL pointer to the image buffer
 */
Bitmap* loadBitmap(const char* filename);

/**
 * @brief Draws a bitmap at the given position
 *
 * @param bmp Bitmap to be drawn
 * @param x Destiny x coord
 * @param y Destiny y coord
 * @param alignment Image alignment
 */
void drawBitmap(Bitmap* bmp, int x, int y, Alignment alignment);
/**
 * @brief Draws a bitmap at the given position for every colour except the alphaColour.
 *
 * @param bmp Bitmap to be drawn
 * @param x Destiny x coord
 * @param y Destiny y coord
 * @param alphaColour Colour to be ignored when drawn (to create transparency)
 * @param mouseBuffer If 1, draws the bitmap to the mouse buffer, else draws to double buffer.
 */
void drawBitmapAlpha(Bitmap* bmp, int x, int y, unsigned long alphaColour,
		int mouseBuffer);

/**
 * @brief Destroys the given bitmap, freeing all resources used by it.
 * @param bmp bitmap to be destroyed
 */
void deleteBitmap(Bitmap* bmp);

/** @} end of Bitmap */
#endif /* __BITMAP_H */
