#include <minix/drivers.h>
#include <sys/video.h>
#include <sys/mman.h>

#include <assert.h>

#include "vt_info.h"

#include "video_txt.h"

/* Private global variables */

static char *video_mem; /* Address to which VRAM is mapped */

static unsigned scr_width; /* Width of screen in columns */
static unsigned scr_lines; /* Height of screen in lines */

void vt_fill(char ch, char attr) {
	int k;
	char *video_ptr;
	video_ptr = video_mem;

	for (k = 0; k < scr_width * scr_lines; k++, video_ptr++) {
		*video_ptr = ch; /* First byte, is for the character to display */
		video_ptr++; /* Point to Second byte of Video RAM*/
		*video_ptr = attr; /* Second byte, is for the attribute of character */
	}

}

void vt_blank() {

	char *video_ptr;
	video_ptr = video_mem;
	int j;
	for (j = 0; j < scr_width * scr_lines; j++, video_ptr++) {
		*video_ptr = 0x00;
		video_ptr++;
		*video_ptr = 0x00;
	}

	//vt_fill(0x00, 0x00);

}

int vt_print_char(char ch, char attr, int r, int c) {

	char *video_ptr;
	video_ptr = video_mem;
	int i;
	int row = r;
	for (i = 0; i < row; i++) {
		video_ptr = video_ptr + scr_width * 2;
	}
	video_ptr = video_ptr + c * 2;

	*video_ptr = ch;
	video_ptr++;
	*video_ptr = attr;

	return 0;
}

int vt_print_string(char *str, char attr, int r, int c) {

	int i, j;
	int len = strlen(str);
	char *video_ptr;
	video_ptr = video_mem;
	for (i = 0; i < r; i++) {
		video_ptr = video_ptr + scr_width * 2;
	}
	video_ptr = video_ptr + c * 2;
	for (j = 0; j < len; j++, video_ptr++) {
		*video_ptr = str[j];
		video_ptr++;
		*video_ptr = attr;
	}

	return 0;
}

int vt_print_int(int num, char attr, int r, int c) {

	char *video_ptr, ch;
	video_ptr = video_mem;
	video_ptr = video_ptr + (c * 2 + r * scr_width * 2);

	int n = num;
	int digits = 0;

	//Count number of digits in the number
	if (num > 0) {
		while (n > 0) {
			n /= 10;
			digits++;
		}
	} else if (num < 0) {
		n = abs(n);
		num = abs(num);
		while (n > 0) {
			n /= 10;
			digits++;
		}
		*video_ptr = '-';
		video_ptr++;
		*video_ptr = attr;
		video_ptr++;
	} else {
		*video_ptr = '0';
		video_ptr++;
		*video_ptr = attr;
		video_ptr++;
		return 0;
	}

	video_ptr = video_ptr + (digits * 2);
	for (digits; digits > 0; digits--) {
		ch = (char) (((int) '0') + (num % 10));
		video_ptr--;
		*video_ptr = attr;
		video_ptr--;
		*video_ptr = ch;
		num /= 10;
	}
	return 0;

}

int vt_draw_frame(int width, int height, char attr, int r, int c) {
	//Checks if frame is within the screen dimensions
	if (c + width >= scr_width){
		printf("%s \n", "ERROR: Frame out of bounds");
		return 1;
	}
	else if (r + height >= scr_lines){
		printf("%s \n", "ERROR: Frame out of bounds");
		return 1;
	}

	int i;
	char *video_ptr;
	video_ptr = video_mem;
	video_ptr = video_ptr + (c * 2 + r * scr_width * 2);

	//Draw upper left corner
	*video_ptr = UL_CORNER;
	video_ptr++;
	*video_ptr = attr;
	video_ptr++;

	//Draw upper bar
	for (i = 0; i < width; i++) {
		*video_ptr = HOR_BAR;
		video_ptr++;
		*video_ptr = attr;
		video_ptr++;
	}

	//Draw upper right corner
	*video_ptr = UR_CORNER;
	video_ptr++;
	*video_ptr = attr;
	video_ptr--;

	//Draw right bar
	for (i = 0; i < height - 2; i++) {
		video_ptr += scr_width * 2;
		*video_ptr = VERT_BAR;
		video_ptr++;
		*video_ptr = attr;
		video_ptr--;
	}
	video_ptr += scr_width * 2;

	//Draw lower right corner
	*video_ptr = LR_CORNER;
	video_ptr++;
	*video_ptr = attr;
	video_ptr -= 3;

	//Draw lower bar
	for (i = 0; i < width; i++) {
		*video_ptr = HOR_BAR;
		video_ptr++;
		*video_ptr = attr;
		video_ptr -= 3;
	}

	//Draw lower left corner
	*video_ptr = LL_CORNER;
	video_ptr++;
	*video_ptr = attr;
	video_ptr--;

	//Draw left bar
	for (i = 0; i < height - 2; i++) {
		video_ptr -= scr_width * 2;
		*video_ptr = VERT_BAR;
		video_ptr++;
		*video_ptr = attr;
		video_ptr--;
	}

	return 0;
}

/*
 * THIS FUNCTION IS FINALIZED, do NOT touch it
 */

char *vt_init(vt_info_t *vi_p) {

	int r;
	struct mem_range mr;

	/* Allow memory mapping */

	mr.mr_base = (phys_bytes)(vi_p->vram_base);
	mr.mr_limit = mr.mr_base + vi_p->vram_size;

	if (OK != (r = sys_privctl(SELF, SYS_PRIV_ADD_MEM, &mr)))
		panic("video_txt: sys_privctl (ADD_MEM) failed: %d\n", r);

	/* Map memory */

	video_mem = vm_map_phys(SELF, (void *) mr.mr_base, vi_p->vram_size);

	if (video_mem == MAP_FAILED)
		panic("video_txt couldn't map video memory");

	/* Save text mode resolution */

	scr_lines = vi_p->scr_lines;
	scr_width = vi_p->scr_width;

	return video_mem;
}
