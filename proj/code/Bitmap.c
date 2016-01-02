#include "Bitmap.h"

#include "stdio.h"
#include <minix/driver.h>
#include "Graphics.h"
#include "Utilities.h"

Bitmap* loadBitmap(const char* filename) {
	// allocating necessary size
	Bitmap* bmp = (Bitmap*) malloc(sizeof(Bitmap));

	// open filename in read binary mode
	FILE *filePtr;
	filePtr = fopen(filename, "rb");
	if (filePtr == NULL)
		return NULL;

	// read the bitmap file header
	BitmapFileHeader bitmapFileHeader;
	fread(&bitmapFileHeader, 2, 1, filePtr);

	// verify that this is a bmp file by check bitmap id
	if (bitmapFileHeader.type != 0x4D42) {
		fclose(filePtr);
		return NULL;
	}

	int rd;
	do {
		if ((rd = fread(&bitmapFileHeader.size, 4, 1, filePtr)) != 1)
			break;
		if ((rd = fread(&bitmapFileHeader.reserved, 4, 1, filePtr)) != 1)
			break;
		if ((rd = fread(&bitmapFileHeader.offset, 4, 1, filePtr)) != 1)
			break;
	} while (0);

	if (rd = !1) {
		fprintf(stderr, "Error reading file\n");
		exit(-1);
	}

	// read the bitmap info header
	BitmapInfoHeader bitmapInfoHeader;
	fread(&bitmapInfoHeader, sizeof(BitmapInfoHeader), 1, filePtr);

	// move file pointer to the begining of bitmap data
	fseek(filePtr, bitmapFileHeader.offset, SEEK_SET);

	// allocate enough memory for the bitmap image data
	unsigned char* bitmapImage = (unsigned char*) malloc(
			bitmapInfoHeader.imageSize);

	// verify memory allocation
	if (!bitmapImage) {
		free(bitmapImage);
		fclose(filePtr);
		return NULL;
	}

	// read in the bitmap image data
	if (bitmapInfoHeader.width % 2 == 0)
		fread(bitmapImage, bitmapInfoHeader.imageSize, 1, filePtr);
	else {
		size_t i;
		uint16_t discard;
		for (i = 0; i < bitmapInfoHeader.height; ++i) {
			fread((uint16_t *) bitmapImage + i * bitmapInfoHeader.width,
					bitmapInfoHeader.width * bitmapInfoHeader.bits / 8, 1,
					filePtr);
			fread(&discard, 2, 1, filePtr);
		}
	}

	// make sure bitmap image data was read
	if (bitmapImage == NULL) {
		fclose(filePtr);
		return NULL;
	}

	// close file and return bitmap image data
	fclose(filePtr);

	bmp->bitmapData = bitmapImage;
	bmp->bitmapInfoHeader = bitmapInfoHeader;

	return bmp;
}

void drawBitmap(Bitmap* bmp, int x, int y, Alignment alignment) {
	if (bmp == NULL)
		return;

	int width = bmp->bitmapInfoHeader.width;
	int drawWidth = width;
	int height = bmp->bitmapInfoHeader.height;

	if (alignment == ALIGN_CENTER)
		x -= width / 2;
	else if (alignment == ALIGN_RIGHT)
		x -= width;

	if (x + width < 0 || x > getHRes() || y + height < 0 || y > getVRes())
		return;

	int xCorrection = 0;
	if (x < 0) {
		xCorrection = -x;
		drawWidth -= xCorrection;
		x = 0;

		if (drawWidth > getHRes())
			drawWidth = getHRes();
	} else if (x + drawWidth >= getHRes()) {
		drawWidth = getHRes() - x;
	}

	char* bufferStartPos;
	char* imgStartPos;

	int i;
	for (i = 0; i < height; i++) {
		int pos = y + height - 1 - i;

		if (pos < 0 || pos >= getVRes())
			continue;

		bufferStartPos = getDoubleBuffer();
		bufferStartPos += x * 2 + pos * getHRes() * 2;

		imgStartPos = bmp->bitmapData + xCorrection * 2 + i * width * 2;

		memcpy(bufferStartPos, imgStartPos, drawWidth * 2);
	}
}

void drawBitmapAlpha(Bitmap* bmp, int x, int y, unsigned long alphaColour) {
	/*int width = bmp->bitmapInfoHeader.width;
	 int height = bmp->bitmapInfoHeader.height;

	 if (x < 0 || x >= getHRes() || y < 0 || y >= getVRes())
	 return;

	 unsigned long colour;

	 int line, column;
	 for (line = 0; line < height; line++) {
	 for (column = 0; column < width; column++) {
	 int pos = (column * 2) + (line * width * 2);

	 unsigned short tmp1 = bmp->bitmapData[pos];
	 unsigned short tmp2 = bmp->bitmapData[pos + 1];
	 unsigned long colour = (tmp1 | (tmp2 << 8));

	 if (colour != alphaColour)
	 if (x + column < getHRes() && y + line < getVRes()) {
	 *getMouseBuffer() = bmp->bitmapData[column + line * width * 2];
	 *(mouseBuffer + 1) = bmp->bitmapData[(column + 1)
	 + line * width * 2];

	 *(getMouseBuffer() + ((hRes * y + line) + x) * bytesPerPixel) = colour
	 & 0xFF;
	 *(getMouseBuffer() + (((hRes * y) + x) * bytesPerPixel) + 1) =
	 (colour >> 8) & 0xFF;
	 }
	 }
	 //putPixelMouseBuffer(x + column, y + line, colour);
	 }*/

	int colorToFilter = (int) alphaColour;

	if (bmp == NULL)
		return;

	char* bufferStartPos;
	unsigned int width = bmp->bitmapInfoHeader.width;
	unsigned int height = bmp->bitmapInfoHeader.height;

	int i, j;
	for (i = 0; i < height; i++) {
		int pos = y + height - 1 - i;

		if (pos < 0 || pos >= getVRes())
			continue;

		bufferStartPos = (char*) getMouseBuffer() + x * 2
				+ (y + height - i) * getHRes() * 2;

		for (j = 0; j < width * 2; j++, bufferStartPos++) {
			if (x + j < 0 || x * 2 + j >= getHRes() * 2)
				continue;

			int pos = j + i * width * 2;
			unsigned short tmp1 = bmp->bitmapData[pos];
			unsigned short tmp2 = bmp->bitmapData[pos + 1];
			unsigned short temp = (tmp1 | (tmp2 << 8));

			//LOG_VAR("pixel color", temp);
			if (temp != colorToFilter) {
				*bufferStartPos = bmp->bitmapData[j + i * width * 2];
				j++;
				bufferStartPos++;
				*bufferStartPos = bmp->bitmapData[j + i * width * 2];
			} else {
				j++;
				bufferStartPos++;
			}
		}
	}
}

void deleteBitmap(Bitmap* bmp) {
	if (bmp == NULL)
		return;

	free(bmp->bitmapData);
	free(bmp);
}
