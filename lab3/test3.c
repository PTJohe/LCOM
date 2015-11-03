#include <minix/syslib.h>
#include <minix/drivers.h>
#include <minix/com.h>
#include <stdio.h>
#include <stdlib.h>
#include <minix/sysutil.h>

#include "i8042.h"
#include "test3.h"
#include "keyboard.h"
#include "timer.h"

int kbd_test_scan(unsigned short ass) {
	int irq_set = kbd_subscribe_int();
	int r;
	int ipc_status;
	int state = 0;
	unsigned long scancode = 0;
	message msg;
	//sys_outb(KBC_IN_BUF, 0xF4);

	while (scancode != MAKE_ESC_CODE) {

		/* Get a request message. */
		if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) {
			printf("driver_receive failed with: %d", r);
			continue;
		}
		if (is_ipc_notify(ipc_status)) { /* received notification */
			switch (_ENDPOINT_P(msg.m_source)) {
			case HARDWARE: /* hardware interrupt notification */
				if (msg.NOTIFY_ARG & irq_set) {
					if (ass == 0)
						scancode = keyboard_int_handler_C();
					else if (ass == 1)
						scancode = keyboard_int_handler_assembly();
					if (scancode == 0xE0) {
						state = 1;
						break;
					} else {
						if (scancode & BIT(7))
							printf("Breakcode: ");
						else
							printf("Makecode: ");

						if (state == 1) {
							printf("0xE0%02X\n", scancode);
							state = 0;
							break;
						} else if (state == 0)
							printf("0x%02X\n", scancode);
					}
				} else
					return EXIT_FAILURE;
			default:
				break; /* no other notifications expected: do nothing */
			}
		} else { /* received a standard message, not a notification */
			/* no standard messages expected: do nothing */
		}
	}
	kbd_unsubscribe_int();
	printf("kbd_test_scan concluido.\n");
	return EXIT_SUCCESS;
}

int kbd_test_leds(unsigned short n, unsigned short *leds) {
	if (kbd_subscribe_int() == EXIT_FAILURE) {
		printf("ERROR: kbd_subscribe_int() failed!");
		return EXIT_FAILURE;
	}

	timer_counter = 0; // initialize "counter" global variable
	int irq_set = timer_subscribe_int();
	if (irq_set < 0) {
		printf("ERROR: timer_subscribe_int failed!");
		return EXIT_FAILURE;
	}

	unsigned long stat = 0;
	unsigned int l;
	int capsLock = 0;
	int scrollLock = 0;
	int numLock = 0;
	int i = 0;

	int interrupt = BIT(irq_set);
	int r;
	int ipc_status;
	message msg;
	while ( 60 >= timer_counter) { /* You may want to use a different condition */
		/* Get a request message. */
		if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) {
			printf("driver_receive failed with: %d", r);
			continue;
		}
		if (is_ipc_notify(ipc_status)) { /* received notification */
			switch (_ENDPOINT_P(msg.m_source)) {
			case HARDWARE: /* hardware interrupt notification */
				if (msg.NOTIFY_ARG & interrupt) {
					if (timer_counter % 60 == 0) {
						if (issue_commandArgument_KBC(KBD_TOGGLE_LEDS,
								(scrollLock | numLock | capsLock) ^ BIT(leds[i]))
								!= EXIT_SUCCESS) {
							printf("ERROR: failed to write to KBD!");
							return EXIT_FAILURE;
						}
						switch (leds[i]) {
						case 0:
							scrollLock = (scrollLock ^ BIT(0));

							break;
						case 1:
							numLock = (numLock ^ BIT(1));
							break;
						case 2:
							capsLock = (capsLock ^ BIT(2));
							break;
						default:
							break;
						}
						i++;
					}
				}
				timer_counter++;
				break;
			default:
				break; /* no other notifications expected: do nothing */
			}
		}
	}
	if (kbd_unsubscribe_int() == EXIT_FAILURE) {
		printf("ERROR: kbd_unsubscribe_int failed!");
		return EXIT_FAILURE;
	}
	if (timer_unsubscribe_int() == EXIT_FAILURE) {
		printf("ERROR: timer_unsubscribe_int failed!");
		return EXIT_FAILURE;
	}
	printf("kbd_test_leds concluido.\n");
	return EXIT_SUCCESS;

}

/*while (i < n) {
 if (issue_commandArgument_KBC(KBD_TOGGLE_LEDS,
 (scrollLock | numLock | capsLock) ^ BIT(leds[i]))
 != EXIT_SUCCESS) {
 printf("ERROR: failed to write to KBD!");
 return EXIT_FAILURE;
 }
 switch (leds[i]) {
 case 0:
 scrollLock = (scrollLock ^ BIT(0));
 break;
 case 1:
 numLock = (numLock ^ BIT(1));
 break;
 case 2:
 capsLock = (capsLock ^ BIT(2));
 break;
 default:
 break;
 }
 i++;
 timer_wait(1);
 }*/

int kbd_test_timed_scan(unsigned short n) {
	int kb_irq = kbd_subscribe_int();
	int timer_irq = timer_subscribe_int();

	if (kb_irq < 0 || timer_irq < 0) {
		printf("ERROR: failed to subscribe interrupts.\n");
		return 1;
	}

	int r;
	int ipc_status;
	int state = 0;
	unsigned long scancode = 0;
	unsigned int time = 0;

	message msg;

	while ((scancode != MAKE_ESC_CODE) && (time < 60 * n)) {
		/* Get a request message. */
		if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) {
			printf("driver_receive failed with: %d", r);
			continue;
		}
		if (is_ipc_notify(ipc_status)) { /* received notification */
			switch (_ENDPOINT_P(msg.m_source)) {
			case HARDWARE: /* hardware interrupt notification */
				if (msg.NOTIFY_ARG & kb_irq) {
					time = 0;
					scancode = keyboard_int_handler_C();
					if (scancode == 0xE0) {
						state = 1;
						break;
					} else {
						if (scancode & BIT(7))
							printf("Breakcode: ");
						else
							printf("Makecode: ");

						if (state == 1) {
							printf("0xE0%02X\n", scancode);
							state = 0;
							break;
						} else if (state == 0)
							printf("0x%02X\n", scancode);
					}

				}
				if (msg.NOTIFY_ARG & timer_irq)
					time++;
				break; /* no other notifications expected: do nothing */
			default:
				break;
			}
		}
	}

	if (kbd_unsubscribe_int() != EXIT_SUCCESS) {
		printf("ERROR: failed to unsubscribe kbd interrupts.\n");
		return 1;
	}
	if (timer_unsubscribe_int() != EXIT_SUCCESS) {
		printf("ERROR: failed to unsubscribe timer interrupts.\n");
		return 1;
	}
	printf("kbd_test_timed_scan concluido.\n");
	return EXIT_SUCCESS;
}

