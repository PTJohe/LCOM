#include "Utilities.h"

// Converts RGB to Hex value.
int RGB(unsigned char r, unsigned char g, unsigned char b) {
	if (r < 0 || r > 255 || g < 0 || g > 255 || b < 0 || b > 255)
		return -1;

	int red, green, blue;
	red = r * 31 / 255;
	green = g * 63 / 255;
	blue = b * 31 / 255;

	return (red << 11) | (green << 5) | blue;
}

// Swaps two values
void swap(int* xi, int* xf) {
	int temp = *xi;
	*xi = *xf;
	*xf = temp;
}

// Round function, introduced in Minix 3.3.0
double round(double x) {
	double t;
	if (x >= 0.0) {
		t = ceil(x);
		if (t - x > 0.5)
			t -= 1.0;
		return (t);
	} else {
		t = ceil(-x);
		if (t + x > 0.5)
			t -= 1.0;
		return (-t);
	}
}

double ceil(double num) {
	int inum = (int) num;
	if (num == (float) inum) {
		return inum;
	}
	return inum + 1;
}

const char* getMousePath(int cursor) {
	char num[256];
	sprintf(num, PATH_IMAGE "mouse%d.bmp", cursor);

	char* path = (char*) malloc(256);
	strcpy(path, num);
	return path;
}
const char* getFontPath(char* character) {
	char num[256];
	sprintf(num, PATH_FONT "%s.bmp", character);

	char* path = (char*) malloc(256);
	strcpy(path, num);
	return path;
}

const char* getImagePath(char* image) {
	char num[256];
	sprintf(num, PATH_IMAGE "%s.bmp", image);

	char* path = (char*) malloc(256);
	strcpy(path, num);
	return path;
}

const char* getStagePath(int number) {
	char num[256];
	sprintf(num, PATH_STAGE "stage%02d.bmp", number);

	char* path = (char*) malloc(256);
	strcpy(path, num);
	return path;
}
