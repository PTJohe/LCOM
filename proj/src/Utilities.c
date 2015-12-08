#include "Utilities.h"

// Converts RGB to Hex value.
int RGB(unsigned char r, unsigned char g, unsigned char b) {
	if (r < 0 || 255 < r || g < 0 || 255 < g || b < 0 || b > 255)
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
