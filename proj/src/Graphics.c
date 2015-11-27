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

static char *video_mem; /* Process address to which VRAM is mapped */
static char *phys_addr; /* Stores physical address of VRAM */

static unsigned h_res; /* Horizontal screen resolution in pixels */
static unsigned v_res; /* Vertical screen resolution in pixels */
static unsigned bits_per_pixel; /* Number of VRAM bits per pixel */

static unsigned int video_mem_size;
static unsigned int bytes_per_pixel;

static char *double_buffer;
static char *mouse_buffer;

void *initGraphics(unsigned short mode) {
	struct reg86u r;
	r.u.w.ax = 0x4F02; // VBE call, function 02 -- set VBE mode
	r.u.w.bx = 1 << 14 | mode; // set bit 14: linear framebuffer
	r.u.b.intno = 0x10;

	vbe_mode_info_t vbe_mode_info;
	if (sys_int86(&r) != OK) {
		printf("set_vbe_mode: sys_int86() failed \n");
	} else if (r.u.w.ax == VBE_FUNCTION_SUPPORTED | VBE_FUNCTION_CALL_SUCCESSFUL) {
		if (vbe_get_mode_info(mode, &vbe_mode_info)) {
			return NULL;
		} else {
			h_res = vbe_mode_info.XResolution;
			v_res = vbe_mode_info.YResolution;
			bits_per_pixel = vbe_mode_info.BitsPerPixel;
			video_mem_size = h_res * v_res * bits_per_pixel / 8;

			//Allow memory mapping
			struct mem_range mr;
			unsigned mr_size;
			mr.mr_base = vbe_mode_info.PhysBasePtr;
			mr.mr_limit = mr.mr_base + video_mem_size;

			if (sys_privctl(SELF, SYS_PRIV_ADD_MEM, &mr))
				return NULL;

			//Map memory
			video_mem = vm_map_phys(SELF, (void *) mr.mr_base, video_mem_size);

			if (video_mem != MAP_FAILED) {
				if ((double_buffer = (char*) malloc(video_mem_size)) != NULL) {
					if ((mouse_buffer = (char*) malloc(video_mem_size)) != NULL) {
						return video_mem;
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
		printf("\tvg_exit(): sys_int86() failed \n");
		return EXIT_FAILURE;
	} else
		return EXIT_SUCCESS;
}

char* getVideoMem() {
	return video_mem;
}

unsigned getHRes() {
	return h_res;
}

unsigned getVRes() {
	return v_res;
}

//==========================================================================================
int fillDisplay(unsigned long color) {
	char *pixel;
	for (pixel = double_buffer;
			pixel < double_buffer + video_mem_size;
			pixel++) {
		*pixel = color;
	}
	memcpy(video_mem, double_buffer, video_mem_size);
	return EXIT_SUCCESS;
}

void putPixel(int x, int y, unsigned long color) {
	//Color pixel of double buffer
	//User is responsible for copying double_buffer into video_mem after drawing
	*(double_buffer + ((h_res * y) + x) * bits_per_pixel / 8) = color;
}

int drawSquare(int xi, int yi, int size, unsigned long color) {
	//Check if coordinates are valid
	if ((xi >= h_res || xi < 0) || (yi >= v_res || yi < 0)) {
		return EXIT_FAILURE;
	}

	//Draw a square the specified position
	int line, column;
	for (line = 0; line < size; line++) {
		for (column = 0; column < size; column++) {
			if ((xi + column < h_res) && (yi + line < v_res)) {
				putPixel((xi + column), (yi + line), color);
			}
		}
	}
	memcpy(video_mem, double_buffer, video_mem_size);

	return EXIT_SUCCESS;
}

int drawLine(int xi, int yi, int xf, int yf, unsigned long color) {
	//Check if coordinates are valid
	if ((xf >= h_res || xf < 0 || xi >= h_res || xi < 0)
			|| (yf >= v_res || yf < 0 || yi >= v_res || yi < 0)) {
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
			putPixel(xi, (int) round(y), color);
		}
	} else {
		if (yi > yf) {
			swap(&xi, &xf);
			swap(&yi, &yf);
			stepX = -stepX;
		}
		double x = xi;
		for (yi; yi <= yf; yi++, x += stepX / m) {
			putPixel((int) round(x), yi, color);
		}
	}
	memcpy(video_mem, double_buffer, video_mem_size);

	return EXIT_SUCCESS;
}

int drawPixmap(int xi, int yi, char* pixmap, int width, int height) {
	//Check if coordinates are valid
	if ((xi >= h_res || xi < 0) || (yi >= v_res || yi < 0)) {
		return EXIT_FAILURE;
	}

	//Draw pixmap at the specified position
	int line, column;
	for (line = 0; line < height; line++) {
		for (column = 0; column < width; column++) {
			if ((xi + column < h_res) && (yi + line < v_res)) {
				putPixel((xi + column), (yi + line),
						pixmap[column + (line * width)]);
			}
		}
	}
	memcpy(video_mem, double_buffer, video_mem_size);

	return EXIT_SUCCESS;
}

int clearPixmap(int xi, int yi, int width, int height) {
	//Check if coordinates are valid
	if ((xi >= h_res || xi < 0) || (yi >= v_res || yi < 0)) {
		return EXIT_FAILURE;
	}

	//Draw a black rectangle in the pixmap's position
	int line, column;
	for (line = 0; line < height; line++) {
		for (column = 0; column < width; column++) {
			if ((xi + column < h_res) && (yi + line < v_res)) {
				putPixel((xi + column), (yi + line), COLOUR_BLACK);
			}
		}
	}
	memcpy(video_mem, double_buffer, video_mem_size);

	return EXIT_SUCCESS;
}
