#include "test5.h"

#include <unistd.h>
#include "constants.h"

#include "timer.h"
#include "keyboard.h"
#include "vbe.h"
#include "video_gr.h"

void *test_init(unsigned short mode, unsigned short delay) {

	//Switch to the specified mode
	char* video_mem = vg_init(mode);

	//Get physical address of video RAM
	vbe_mode_info_t vbe_mode_info;
	vbe_get_mode_info(mode, &vbe_mode_info);

	//Wait delay seconds.
	unsigned char timer_hook_bit;
	if (timer_hook_bit = timer_subscribe_int() == EXIT_FAILURE) {
		vg_exit();
		printf("ERROR: Failed to subscribe timer interrupt.");
		return NULL;
	}
	int r, ipc_status;
	message msg;
	unsigned counter = 0;
	while (counter < delay * TIMER_DEFAULT_FREQ) {
		//Get a request message.
		if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) {
			//Driver receive fail
			continue;
		}
		if (is_ipc_notify(ipc_status)) { //Received notification
			if (_ENDPOINT_P(msg.m_source) == HARDWARE) //Hardware interrupt notification
					{
				if (msg.NOTIFY_ARG & BIT(timer_hook_bit)) {
					counter++;
				}
			}
		}
	}
	timer_unsubscribe_int();

	//Return to text mode
	vg_exit();

	printf("\nVRAM physical address: 0x%X\n", vbe_mode_info.PhysBasePtr);
	return video_mem;
}

int test_square(unsigned short x, unsigned short y, unsigned short size,
		unsigned long color) {

	/* To be completed */

}

int test_line(unsigned short xi, unsigned short yi, unsigned short xf,
		unsigned short yf, unsigned long color) {

	vg_init(0x105);

	/*unsigned char kbd_hook_bit;
	if (kbd_hook_bit = kbd_subscribe_int() == EXIT_FAILURE) {
		vg_exit();
		printf("ERROR: Failed to subscribe keyboard interrupt!\n");
		return EXIT_FAILURE;
	}*/
	unsigned h_res = getHRes();
	unsigned v_res = getVRes();
	if ((xf >= h_res || xi < 0) || (yf >= v_res || yi < 0)) {
		vg_exit();
		kbd_unsubscribe_int();
		printf("ERROR: Invalid coordinates!\n");
		return EXIT_FAILURE;
	}

	char *video_mem;

	/*
	 * Adapted from DDA Line Drawing Algorithm
	 */
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
			video_mem = getVideoMem();
			video_mem = video_mem + (h_res * (int) round(y)) + xi;
			*video_mem = color;
		}
	} else {
		if (yi > yf) {
			swap(&xi, &xf);
			swap(&yi, &yf);
			stepX = -stepX;
		}
		double x = xi;
		for (yi; yi <= yf; yi++, x += stepX / m) {
			video_mem = getVideoMem();
			video_mem = video_mem + (h_res * yi) + (int) round(x);
			*video_mem = color;
		}
	}

	//Wait until user presses ESC
	int r, ipc_status;
	message msg;

	sleep(2);

	//kbd_unsubscribe_int();
	vg_exit();
	return EXIT_SUCCESS;
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

