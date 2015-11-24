#include <minix/syslib.h>
#include <minix/drivers.h>
#include <machine/int86.h>

#include "vbe.h"
#include "constants.h"

#include "lmlib.h"

#define LINEAR_MODEL_BIT 14

#define PB2BASE(x) (((x) >> 4) & 0x0F000)
#define PB2OFF(x) ((x) & 0x0FFFF)

int vbe_get_mode_info(unsigned short mode, vbe_mode_info_t *vmi_p) {
	mmap_t video;

	if (lm_alloc(sizeof(vbe_mode_info_t), &video) == NULL) {
		return EXIT_FAILURE;
	}

	struct reg86u info;

	info.u.b.intno = VBE_INTERRUPT;
	info.u.b.ah = VBE_FUNCTION;
	info.u.b.al = VBE_MODE_INFO;
	info.u.w.cx = 1 << LINEAR_MODEL_BIT | mode;
	info.u.w.es = PB2BASE(video.phys);
	info.u.w.di = PB2OFF(video.phys);

	if (sys_int86(&info) == OK) {
		if (info.u.w.ax == VBE_FUNCTION_SUPPORTED | VBE_FUNCTION_CALL_SUCCESSFUL) {
			memcpy(vmi_p, video.virtual, 256);
			lm_free(&video);
			return EXIT_SUCCESS;
		}
	}

	lm_free(&video);
	return EXIT_FAILURE;
}

int vbe_get_info_block(vbe_info_block_t *vmi_p) {
	mmap_t video;

	if (lm_alloc(sizeof(vbe_info_block_t), &video) == NULL) {
		return EXIT_FAILURE;
	}

	struct reg86u info;

	info.u.b.intno = VBE_INTERRUPT;
	info.u.b.ah = VBE_FUNCTION;
	info.u.b.al = VBE_CONTROLLER_INFO;
	info.u.w.ax = 0x4F00;
	info.u.w.es = PB2BASE(video.phys);
	info.u.w.di = PB2OFF(video.phys);

	if (sys_int86(&info) != OK) {
		printf("ERROR: Could not get VBE mode info!\n");
		lm_free(&video);
		return EXIT_FAILURE;
	}
	memcpy(vmi_p, video.virtual, 256);
	lm_free(&video); //free memory
	return EXIT_SUCCESS;
}
