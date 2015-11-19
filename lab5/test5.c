#include "test5.h"
#include <unistd.h>

void *test_init(unsigned short mode, unsigned short delay) {
	
	char* video_mem = (char*) vg_init(mode);
	sleep(delay);
	vg_exit();
	printf("VRAM physical address 0x%02X", video_mem);
	//printf() to print the VRAM physical address of the specified mode (use VBE function 0x01 return vbe mode information)
	return video_mem;
	
}


int test_square(unsigned short x, unsigned short y, unsigned short size, unsigned long color) {
	
	/* To be completed */
	
}

int test_line(unsigned short xi, unsigned short yi, 
		           unsigned short xf, unsigned short yf, unsigned long color) {
	
	/* To be completed */
	
}

int test_xpm(unsigned short xi, unsigned short yi, char *xpm[]) {
	
	/* To be completed */
	
}	

int test_move(unsigned short xi, unsigned short yi, char *xpm[], 
				unsigned short hor, short delta, unsigned short time) {
	
	/* To be completed */
	
}					

int test_controller() {
	
	/* To be completed */
	
}					
	
