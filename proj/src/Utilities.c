#include "Utilities.h"


// Converts RGB to Hex value.
unsigned long RGB(int r, int g, int b) {
	return ((r & 0xFF) << 16) + ((g & 0xFF) << 8) + (b & 0xFF);
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
