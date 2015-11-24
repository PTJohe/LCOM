#include "test5.h"

#include <unistd.h>
#include "constants.h"

#include "timer.h"
#include "keyboard.h"
#include "vbe.h"
#include "video_gr.h"
#include "read_xpm.h"
#include "pixmap.h"
#include "sprite.h"

void *test_init(unsigned short mode, unsigned short delay) {

	//Check if mode is a valid VBE mode
	if (mode < 0x100 || mode > 0x11B) {
		printf("ERROR: Non-VBE mode!\n");
		return NULL;
	}

	//Switch to the specified mode
	char* video_mem = vg_init(mode);
	if (video_mem == NULL) {
		printf("ERROR: Could not initialize video module!\n");
		return NULL;
	}

	//Get physical address of video RAM
	vbe_mode_info_t vbe_mode_info;
	vbe_get_mode_info(mode, &vbe_mode_info);
	if (&vbe_mode_info < 0) {
		printf("ERROR: Could not get VBE mode info!\n");
		return NULL;
	}

	//Subscribe timer interrupt
	int timer_hook_bit;
	if (timer_hook_bit = timer_subscribe_int() < 0) {
		vg_exit();
		printf("ERROR: Failed to subscribe timer interrupt!\n");
		return NULL;
	}

	int r, ipc_status;
	message msg;
	unsigned counter = 0;

	//Wait delay seconds.
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

	//Unsubscribe timer interrupt
	if (timer_unsubscribe_int() == EXIT_FAILURE) {
		vg_exit();
		printf("ERROR: Failed to unsubscribe timer interrupt!\n");
		return NULL;
	}

	//Return to text mode
	vg_exit();

	printf("\nVRAM physical address: 0x%X\n", vbe_mode_info.PhysBasePtr);
	printf("lab5::test_init() concluido.\n");
	return video_mem;
}

int test_square(unsigned short x, unsigned short y, unsigned short size,
		unsigned long color) {

	//Switch to the 0x105 mode
	char* video_mem = vg_init(MODE_1024_768);
	if (video_mem == NULL) {
		printf("ERROR: Could not initialize video module!\n");
		return EXIT_FAILURE;
	}

	//Check if coordinates are valid
	unsigned h_res = getHRes();
	unsigned v_res = getVRes();
	if ((x >= h_res || x < 0) || (y >= v_res || y < 0)) {
		vg_exit();
		printf("ERROR: Invalid coordinates!\n");
		return EXIT_FAILURE;
	}

	//Draw a square at the specified position
	int line, column;
	for (line = 0; line < size; line++) {
		for (column = 0; column < size; column++) {
			if ((x + column < h_res) && (y + line < v_res)) {
				video_mem = getVideoMem();
				video_mem = video_mem + (h_res * (y + line)) + (x + column);
				*video_mem = color;
			}
		}
	}

	//Subscribe keyboard interrupt
	int kbd_hook_bit = kbd_subscribe_int();
	if (kbd_hook_bit == EXIT_FAILURE) {
		vg_exit();
		printf("ERROR: Failed to subscribe keyboard interrupt!\n");
		return EXIT_FAILURE;
	}

	int r, ipc_status;
	message msg;
	unsigned long scancode = 0;

	//Wait until user presses ESC
	while (scancode != BREAK_ESC_CODE) {
		//Get a request message.
		if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) {
			printf("driver_receive failed with: %d", r);
			continue;
		}
		if (is_ipc_notify(ipc_status)) { //received notification
			switch (_ENDPOINT_P(msg.m_source)) {
			case HARDWARE: //hardware interrupt notification
				if (msg.NOTIFY_ARG & kbd_hook_bit) {
					scancode = keyboard_int_handler_C();
				}
				break;
			default:
				break; //no other notifications expected: do nothing
			}
		}
	}

	//Unsubscribe keyboard interrupt
	if (kbd_unsubscribe_int() == EXIT_FAILURE) {
		vg_exit();
		printf("ERROR: Failed to unsubscribe keyboard interrupt!\n");
		return EXIT_FAILURE;
	}

	//Return to text mode
	vg_exit();

	printf("lab5::test_square() concluido.\n");
	return EXIT_SUCCESS;
}

int test_line(unsigned short xi, unsigned short yi, unsigned short xf,
		unsigned short yf, unsigned long color) {

	//Switch to the 0x105 mode
	char* video_mem = vg_init(MODE_1024_768);
	if (video_mem == NULL) {
		printf("ERROR: Could not initialize video module!\n");
		return EXIT_FAILURE;
	}

	//Check if coordinates are valid
	unsigned h_res = getHRes();
	unsigned v_res = getVRes();
	if ((xf >= h_res || xf < 0 || xi >= h_res || xi < 0)
			|| (yf >= v_res || yf < 0 || yi >= v_res || yi < 0)) {
		vg_exit();
		printf("ERROR: Invalid coordinates!\n");
		return EXIT_FAILURE;
	}

	//Adapted from DDA Line Drawing Algorithm
	//Draw a line from the initial position to a specified position
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

	//Subscribe keyboard interrupt
	int kbd_hook_bit = kbd_subscribe_int();
	if (kbd_hook_bit == EXIT_FAILURE) {
		vg_exit();
		printf("ERROR: Failed to subscribe keyboard interrupt!\n");
		return EXIT_FAILURE;
	}

	int r, ipc_status;
	message msg;
	unsigned long scancode = 0;

	//Wait until user presses ESC
	while (scancode != BREAK_ESC_CODE) {
		//Get a request message.
		if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) {
			printf("driver_receive failed with: %d", r);
			continue;
		}
		if (is_ipc_notify(ipc_status)) { //received notification
			switch (_ENDPOINT_P(msg.m_source)) {
			case HARDWARE: //hardware interrupt notification
				if (msg.NOTIFY_ARG & kbd_hook_bit) {
					scancode = keyboard_int_handler_C();
				}
				break;
			default:
				break; //no other notifications expected: do nothing
			}
		}
	}

	//Unsubscribe keyboard interrupt
	if (kbd_unsubscribe_int() == EXIT_FAILURE) {
		vg_exit();
		printf("ERROR: Failed to unsubscribe keyboard interrupt!\n");
		return EXIT_FAILURE;
	}

	//Return to text mode
	vg_exit();

	printf("lab5::test_line() concluido.\n");
	return EXIT_SUCCESS;
}

int test_xpm(unsigned short xi, unsigned short yi, char *xpm[]) {

	//Switch to the 0x105 mode
	char* video_mem = vg_init(MODE_1024_768);
	if (video_mem == NULL) {
		printf("ERROR: Could not initialize video module!\n");
		return EXIT_FAILURE;
	}

	//Draw the XPM at the specified position
	int width, height;
	char* pixmap = read_xpm(xpm, &width, &height);

	//Check if the pixmap exists
	if (pixmap == NULL) {
		vg_exit();
		printf("ERROR: Failed to read xpm!\n");
		return EXIT_FAILURE;
	}

	//Check if coordinates are valid
	unsigned h_res = getHRes();
	unsigned v_res = getVRes();
	if ((xi >= h_res || xi < 0) || (yi >= v_res || yi < 0)) {
		vg_exit();
		printf("ERROR: Invalid coordinates!\n");
		return EXIT_FAILURE;
	}

	//Draw XPM
	drawPixmap((int) xi, (int) yi, pixmap, width, height);

	//Subscribe keyboard interrupt
	int kbd_hook_bit = kbd_subscribe_int();
	if (kbd_hook_bit == EXIT_FAILURE) {
		vg_exit();
		printf("ERROR: Failed to subscribe keyboard interrupt!\n");
		return EXIT_FAILURE;
	}

	int r, ipc_status;
	message msg;
	unsigned long scancode = 0;

	//Wait until user presses ESC
	while (scancode != BREAK_ESC_CODE) {
		//Get a request message.
		if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) {
			printf("driver_receive failed with: %d", r);
			continue;
		}
		if (is_ipc_notify(ipc_status)) { //received notification
			switch (_ENDPOINT_P(msg.m_source)) {
			case HARDWARE: //hardware interrupt notification
				if (msg.NOTIFY_ARG & kbd_hook_bit) {
					scancode = keyboard_int_handler_C();
				}
				break;
			default:
				break; //no other notifications expected: do nothing
			}
		}
	}

	//Unsubscribe keyboard interrupt
	if (kbd_unsubscribe_int() == EXIT_FAILURE) {
		vg_exit();
		printf("ERROR: Failed to unsubscribe keyboard interrupt!\n");
		return EXIT_FAILURE;
	}

	//Return to text mode
	vg_exit();

	printf("lab5::test_xpm() concluido.\n");
	return EXIT_SUCCESS;
}

int test_move(unsigned short xi, unsigned short yi, char *xpm[],
		unsigned short hor, short delta, unsigned short time) {

	//Switch to the 0x105 mode
	char* video_mem = vg_init(MODE_1024_768);
	if (video_mem == NULL) {
		printf("ERROR: Could not initialize video module!\n");
		return EXIT_FAILURE;
	}

	//Check if coordinates are valid
	unsigned h_res = getHRes();
	unsigned v_res = getVRes();
	if ((xi >= h_res || xi < 0) || (yi >= v_res || yi < 0)) {
		vg_exit();
		printf("ERROR: Invalid coordinates!\n");
		return EXIT_FAILURE;
	}

	//Check if direction is valid. 0 = Horizontal, 1 = Vertical, 2 = Diagonal
	if (hor != 0 && hor != 1 && hor != 2) {
		vg_exit();
		printf(
				"ERROR: Invalid direction! 0 = Horizontal, 1 = Vertical, 2 = Diagonal\n");
		return EXIT_FAILURE;
	}

	//Subscribe timer and keyboard interrupts
	unsigned char timer_hook_bit;
	if (timer_hook_bit = timer_subscribe_int() < 0) {
		vg_exit();
		printf("ERROR: Failed to subscribe timer interrupt!\n");
		return EXIT_FAILURE;
	}
	int kbd_hook_bit = kbd_subscribe_int();
	if (kbd_hook_bit == EXIT_FAILURE) {
		vg_exit();
		printf("ERROR: Failed to subscribe keyboard interrupt!\n");
		return EXIT_FAILURE;
	}

	//Create sprite to be drawn
	Sprite *sprite = create_sprite(xpm, xi, yi);
	float speed = delta / (time * TIMER_DEFAULT_FREQ);

	int counter = 0;
	int r, ipc_status;
	message msg;
	unsigned long scancode = 0;

	//Wait until user presses ESC
	while (scancode != BREAK_ESC_CODE) {
		//Get a request message.
		if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) {
			printf("driver_receive failed with: %d", r);
			continue;
		}
		if (is_ipc_notify(ipc_status)) { //received notification
			switch (_ENDPOINT_P(msg.m_source)) {
			case HARDWARE: //hardware interrupt notification
				if (msg.NOTIFY_ARG & kbd_hook_bit) {
					scancode = keyboard_int_handler_C();
				} else if (msg.NOTIFY_ARG & BIT(timer_hook_bit)) {
					counter++;
					//Moves the sprite during the specified time
					if (counter / 60 < time) {
						//Horizontal movement
						if (hor == 0) {
							clearPixmap(
									(int) (sprite->x + (speed * (counter - 1))),
									sprite->y, sprite->width, sprite->height);
							drawPixmap((int) (sprite->x + (speed * counter)),
									sprite->y, sprite->map, sprite->width,
									sprite->height);
						}
						//Vertical movement
						else if (hor == 1) {
							clearPixmap(sprite->x,
									(int) (sprite->y + (speed * (counter - 1))),
									sprite->width, sprite->height);
							drawPixmap(sprite->x,
									(int) (sprite->y + (speed * counter)),
									sprite->map, sprite->width, sprite->height);
						}
						//Diagonal movement
						else if (hor == 2) {
							clearPixmap(
									(int) (sprite->x + (speed * (counter - 1))),
									(int) (sprite->y + (speed * (counter - 1))),
									sprite->width, sprite->height);
							drawPixmap((int) (sprite->x + (speed * counter)),
									(int) (sprite->y + (speed * counter)),
									sprite->map, sprite->width, sprite->height);
						}
					}
				}
				break;
			default:
				break; //no other notifications expected: do nothing
			}
		}
	}

	//Unsubscribe timer and keyboard interrupts
	if (timer_unsubscribe_int() == EXIT_FAILURE) {
		kbd_unsubscribe_int();
		vg_exit();
		printf("ERROR: Failed to unsubscribe timer interrupt!\n");
		return EXIT_FAILURE;
	}
	if (kbd_unsubscribe_int() == EXIT_FAILURE) {
		vg_exit();
		printf("ERROR: Failed to unsubscribe keyboard interrupt!\n");
		return EXIT_FAILURE;
	}

	//Return to text mode
	vg_exit();

	printf("lab5::test_move() concluido.\n");
	return EXIT_SUCCESS;
}

int test_controller() {

	if (lm_init() == NULL)
		return EXIT_FAILURE;

	uint16_t *VideoModeList;
	unsigned nr_of_video_modes;
	vbe_info_block_t vib;
	vbe_get_info_block(&vib, &VideoModeList, &nr_of_video_modes);

	printf("Capabilities: 0x%X\n", vib.Capabilities);
	if (vib.Capabilities & BIT(0) == 0)
		printf("DAC is fixed width, with 6 bits per primary color.\n");
	else
		printf("DAC width is switchable to 8 bits per primary color.\n");
	if (vib.Capabilities & BIT(1) == 0)
		printf("Controller is VGA compatible.\n");
	else
		printf("Controller isn't VGA compatible.\n");
	if (vib.Capabilities & BIT(2) == 0)
		printf("Normal RAMDAC operation.\n");
	else
		printf(
				"When programming large block of information to the RAMDAC, use the blank bit in Function 09h.\n");

	printf("Video modes (in hexadecimal) supported: ");
	if (nr_of_video_modes > 0)
		printf("0x%X\n", VideoModeList[0]);
	size_t m;
	for (m = 1; m < nr_of_video_modes; m++)
		printf("0x%X\n", VideoModeList[m]);
	free(VideoModeList);

	printf("Size of VRAM memory: %lu kb\n", vib.TotalMemory * 64);

	printf("\nlab5::test_controller() concluido.\n");
	return EXIT_SUCCESS;
}

