#include <minix/syslib.h>
#include <minix/drivers.h>
#include <machine/int86.h>
#include <sys/mman.h>
#include <sys/types.h>
#include "math.h"

#include "Graphics.h"
#include "Utilities.h"
#include "VBE.h"

/* Private global variables */

static char *videoMem; /* Process address to which VRAM is mapped */
static char *physAddr; /* Stores physical address of VRAM */

static unsigned hRes; /* Horizontal screen resolution in pixels */
static unsigned vRes; /* Vertical screen resolution in pixels */
static unsigned bitsPerPixel; /* Number of VRAM bits per pixel */

static unsigned int videoMemSize;
static unsigned int bytesPerPixel;

static char *doubleBuffer;
static char *mouseBuffer;

void *initGraphics(unsigned short mode) {
	struct reg86u r;
	r.u.w.ax = 0x4F02; // VBE call, function 02 -- set VBE mode
	r.u.w.bx = 1 << 14 | mode; // set bit 14: linear framebuffer
	r.u.b.intno = 0x10;

	vbe_mode_info_t vbeModeInfo;
	if (sys_int86(&r) != OK) {
		printf("\ninitGraphics: sys_int86() failed \n");
	} else if (r.u.w.ax == VBE_FUNCTION_SUPPORTED | VBE_FUNCTION_CALL_SUCCESSFUL) {
		if (vbe_get_mode_info(mode, &vbeModeInfo)) {
			return NULL;
		} else {
			hRes = vbeModeInfo.XResolution;
			vRes = vbeModeInfo.YResolution;
			bitsPerPixel = vbeModeInfo.BitsPerPixel;
			videoMemSize = hRes * vRes * bitsPerPixel / 8;

			//Allow memory mapping
			struct mem_range mr;
			mr.mr_base = vbeModeInfo.PhysBasePtr;
			mr.mr_limit = mr.mr_base + videoMemSize;

			if (sys_privctl(SELF, SYS_PRIV_ADD_MEM, &mr))
				return NULL;

			//Map memory
			videoMem = vm_map_phys(SELF, (void *) mr.mr_base, videoMemSize);

			if (videoMem != MAP_FAILED) {
				if ((doubleBuffer = (char*) malloc(videoMemSize)) != NULL) {
					if ((mouseBuffer = (char*) malloc(videoMemSize)) != NULL) {
						return videoMem;
					}
				}
			}
		}
	}
	return NULL;
}

int exitGraphics() {
	struct reg86u reg86;

	reg86.u.b.intno = 0x10; /* BIOS video services */
	reg86.u.b.ah = 0x00; /* Set Video Mode function */
	reg86.u.b.al = 0x03; /* 80x25 text mode*/

	if (sys_int86(&reg86) != OK) {
		printf("\nexitGraphics(): sys_int86() failed \n");
		return EXIT_FAILURE;
	} else
		return EXIT_SUCCESS;
}

char* getVideoMem() {
	return videoMem;
}

unsigned getHRes() {
	return hRes;
}

unsigned getVRes() {
	return vRes;
}

void copyDoubleBuffer() {
	memcpy(mouseBuffer, doubleBuffer, videoMemSize);
}
void copyMouseBuffer() {
	memcpy(videoMem, mouseBuffer, videoMemSize);
}

//==========================================================================================
int fillDisplay(unsigned long colour) {
	char *pixel;
	for (pixel = doubleBuffer; pixel < doubleBuffer + videoMemSize; pixel++) {
		*pixel = colour;
	}
	return EXIT_SUCCESS;
}

void putPixel(int x, int y, unsigned long colour) {
	//Color pixel of double buffer
	*(doubleBuffer + ((hRes * y) + x) * bitsPerPixel / 8) = colour;
}

int drawRectangle(int xi, int yi, int xf, int yf, unsigned long colour) {
	//Check if coordinates are valid
	if ((xi >= hRes || xi < 0) || (xf >= hRes || xf < 0)
			|| (yi >= vRes || yi < 0) || (yf >= vRes || yf < 0)) {
		return EXIT_FAILURE;
	}

	//Adjust coordinates
	if (yi > yf)
		swap(&yi, &yf);
	if (xi > xf)
		swap(&xi, &xf);

	//Draw a rectangle the specified position
	int line, column;
	for (line = 0; line < (yf - yi); line++) {
		for (column = 0; column < (xf - xi); column++) {
			if ((xi + column < hRes) && (yi + line < vRes)) {
				putPixel((xi + column), (yi + line), colour);
			}
		}
	}

	return EXIT_SUCCESS;
}

int drawSquare(int xi, int yi, int size, unsigned long colour) {
	//Check if coordinates are valid
	if ((xi >= hRes || xi < 0) || (yi >= vRes || yi < 0)) {
		return EXIT_FAILURE;
	}

	//Draw a square the specified position
	int line, column;
	for (line = 0; line < size; line++) {
		for (column = 0; column < size; column++) {
			if ((xi + column < hRes) && (yi + line < vRes)) {
				putPixel((xi + column), (yi + line), colour);
			}
		}
	}

	return EXIT_SUCCESS;
}

int drawLine(int xi, int yi, int xf, int yf, unsigned long colour) {
	//Check if coordinates are valid
	if ((xf >= hRes || xf < 0 || xi >= hRes || xi < 0)
			|| (yf >= vRes || yf < 0 || yi >= vRes || yi < 0)) {
		return EXIT_FAILURE;
	}

	//Adapted from DDA Line Drawing Algorithm
	double dX = abs(xf - xi);
	double dY = abs(yf - yi);

	double m = dY / dX;

	int stepX, stepY;
	if (xf >= xi)
		stepX = 1;
	else
		stepX = -1;
	if (yf >= yi)
		stepY = 1;
	else
		stepY = -1;

	if (m <= 1) {
		if (xi > xf) {
			swap(&xi, &xf);
			swap(&yi, &yf);
			stepY = -stepY;
		}
		double y = yi;
		for (xi; xi <= xf; xi++, y += stepY * m) {
			putPixel(xi, (int) y, colour);
		}
	} else {
		if (yi > yf) {
			swap(&xi, &xf);
			swap(&yi, &yf);
			stepX = -stepX;
		}
		double x = xi;
		for (yi; yi <= yf; yi++, x += stepX / m) {
			putPixel((int) x, yi, colour);
		}
	}

	return EXIT_SUCCESS;
}

int drawCircle(int xc, int yc, int radius, unsigned long colour) {

	int r2 = radius * radius;
	int area = r2 << 2;
	int rr = radius << 1;
	int i;
	for (i = 0; i < area; i++) {
		int tx = (i % rr) - radius;
		int ty = (i / rr) - radius;

		if (tx * tx + ty * ty <= r2)
			putPixel(xc + tx, yc + ty, colour);
	}
	return 0;
}

int drawPixmap(int xi, int yi, char* pixmap, int width, int height) {
	//Check if coordinates are valid
	if ((xi >= hRes || xi < 0) || (yi >= vRes || yi < 0)) {
		return EXIT_FAILURE;
	}

	//Draw pixmap at the specified position
	int line, column;
	for (line = 0; line < height; line++) {
		for (column = 0; column < width; column++) {
			if ((xi + column < hRes) && (yi + line < vRes)) {
				putPixel((xi + column), (yi + line),
						pixmap[column + (line * width)]);
			}
		}
	}

	return EXIT_SUCCESS;
}

int clearPixmap(int xi, int yi, int width, int height) {
	//Check if coordinates are valid
	if ((xi >= hRes || xi < 0) || (yi >= vRes || yi < 0)) {
		return EXIT_FAILURE;
	}

	//Draw a black rectangle in the pixmap's position
	int line, column;
	for (line = 0; line < height; line++) {
		for (column = 0; column < width; column++) {
			if ((xi + column < hRes) && (yi + line < vRes)) {
				putPixel((xi + column), (yi + line), COLOUR_BLACK);
			}
		}
	}

	return EXIT_SUCCESS;
}
