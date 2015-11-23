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
		if (info.u.w.ax == VBE_FUNCTION_SUPPORTED
				| VBE_FUNCTION_CALL_SUCCESSFUL) {
			memcpy(vmi_p, video.virtual, 256);
			lm_free(&video);
			return EXIT_SUCCESS;
		}
	}

	lm_free(&video);
	return EXIT_FAILURE;
}

int vbe_get_info_block(vbe_info_block_t *vib_p, uint16_t **VideoModeList, unsigned *nr_of_video_modes){
	mmap_t video;

	if (lm_alloc(sizeof(vbe_info_block_t), &video) == NULL) {
			return EXIT_FAILURE;
		}

	struct reg86u info;

	info.u.b.intno = VBE_INTERRUPT;
	info.u.b.ah = VBE_FUNCTION;
	info.u.b.al = VBE_CONTROLLER_INFO;
	info.u.w.es = PB2BASE(video.phys);
	info.u.w.di = PB2OFF(video.phys);

	if (sys_int86(&info) == OK) {
			if (info.u.w.ax == VBE_FUNCTION_SUPPORTED | VBE_FUNCTION_CALL_SUCCESSFUL) {
				memcpy(vib_p, video.virtual, 512); // "the information block is 512 bytes in size" - page 12 "vbe20.pdf"
				//lm_free(&video);
				return EXIT_SUCCESS;
			}
			else
				return EXIT_FAILURE;
	}
	else
		return EXIT_FAILURE;

	//*vib_p = *((vbe_info_block_t *)video.virtual);

	if (memcmp(vib_p->VbeSignature, "VESA", sizeof(vib_p->VbeSignature)))
		return EXIT_FAILURE;

	void *ptr = (void *)((vib_p->VideoModePtr & 0xFFFF0000) >> 12);
	ptr += PB2OFF(vib_p->VideoModePtr);
	ptr += (uint32_t)video.virtual & 0xF0000000;

	lm_free(&video);

	uint16_t *modes = ptr; // Each mode number occupies one word (16 bits). The list of mode numbers is	terminated by a -1 (0FFFFh) - page 14 "vbe20.pdf"
	*nr_of_video_modes = 0;
	size_t m;
	for (m = 0; *modes != -1; modes++) // The list of mode numbers is terminated by a -1 (0FFFFh) - page 14 "vbe20.pdf"
		*nr_of_video_modes++;
	if ((*VideoModeList = lm_alloc((*nr_of_video_modes) * sizeof(uint16_t), &video)) == NULL)
		return EXIT_FAILURE;
	for (m = 0, modes = ptr; m < *nr_of_video_modes; modes++, m++)
		(*VideoModeList)[m] = *modes;
	return EXIT_SUCCESS;
}
