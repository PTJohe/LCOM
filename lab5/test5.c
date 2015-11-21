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
	if (timer_hook_bit = timer_subscribe_int() < 0)
		return;
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

