#include <minix/syslib.h>
#include <minix/drivers.h>
#include <machine/int86.h>
#include <sys/mman.h>
#include <sys/types.h>

#include "video_gr.h"
#include "constants.h"
#include "vbe.h"

/* Constants for VBE 0x105 mode */

/* The physical address may vary from VM to VM.
 * At one time it was 0xD0000000
 *  #define VRAM_PHYS_ADDR    0xD0000000 
 * Currently on lab B107 is 0xF0000000
 * Better run my version of lab5 as follows:
 *     service run `pwd`/lab5 -args "mode 0x105"
 */
#define VRAM_PHYS_ADDR	0xF0000000
#define H_RES             1024
#define V_RES		  768
#define BITS_PER_PIXEL	  8

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

void *vg_init(unsigned short mode) {
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

			//Allow memory mapping
			struct mem_range mr;
			unsigned mr_size;
			mr.mr_base = vbe_mode_info.PhysBasePtr;
			mr_size = h_res * v_res * bits_per_pixel;
			mr.mr_limit = mr.mr_base + mr_size;

			if (sys_privctl(SELF, SYS_PRIV_ADD_MEM, &mr))
				return NULL;

			//Map memory
			video_mem = vm_map_phys(SELF, (void *) mr.mr_base, mr_size);

			if (video_mem != MAP_FAILED) {
				if ((double_buffer = malloc(h_res * v_res * bits_per_pixel / 8))
						!= NULL) {
					if ((mouse_buffer = malloc(
							h_res * v_res * bits_per_pixel / 8)) != NULL) {
						return video_mem;
					}
				}
			}
		}
	}
	return NULL;
}

int vg_exit() {
	struct reg86u reg86;

	reg86.u.b.intno = 0x10; /* BIOS video services */
	reg86.u.b.ah = 0x00; /* Set Video Mode function */
	reg86.u.b.al = 0x03; /* 80x25 text mode*/

	if (sys_int86(&reg86) != OK) {
		printf("\tvg_exit(): sys_int86() failed \n");
		return 1;
	} else
		return 0;
}
