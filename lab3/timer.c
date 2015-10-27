#include "timer.h"
#include "i8042.h"

#include <minix/syslib.h>
#include <minix/drivers.h>
#include <minix/com.h>
#include <stdio.h>
#include <stdlib.h>

#include <minix/drivers.h>

int timer_subscribe_int(void) {
	hook_id_timer = 0;
	int bitmask = BIT(hook_id_timer);
	if (sys_irqsetpolicy(TIMER0_IRQ, IRQ_REENABLE, &hook_id_timer) == OK
			&& sys_irqenable(&hook_id_timer) == OK)
		return bitmask;
	else
		return EXIT_FAILURE;
}

int timer_unsubscribe_int() {

	if (sys_irqrmpolicy(&hook_id_timer) != OK)
		return EXIT_FAILURE;
	else
		return EXIT_SUCCESS;
}

void timer_int_handler() {
	timer_counter = timer_counter + 1;
}

int timer_wait(unsigned long time) {
	if (time == 0) {
		printf("Error: Time interval must be positive.\n");
		return EXIT_FAILURE;
	}

	timer_counter = 0; // initialize "counter" global variable
	int irq_set = timer_subscribe_int();
	if (irq_set < 0) {
		printf("ERROR: timer_subscribe_int failed!");
		return EXIT_FAILURE;
	}

	int interrupt = BIT(irq_set);
	int r;
	int ipc_status;
	message msg;
	while ((time * 60) >= timer_counter) { /* You may want to use a different condition */
		/* Get a request message. */
		if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) {
			printf("driver_receive failed with: %d", r);
			continue;
		}
		if (is_ipc_notify(ipc_status)) { /* received notification */
			switch (_ENDPOINT_P(msg.m_source)) {
			case HARDWARE: /* hardware interrupt notification */
				if (msg.NOTIFY_ARG & interrupt) {
					timer_int_handler();
				}
				break;
			default:
				break; /* no other notifications expected: do nothing */
			}
		} else { /* received a standard message, not a notification */
			/* no standard messages expected: do nothing */
		}
	}
	if (timer_unsubscribe_int() != 0) {
		printf("ERROR: timer_unsubscribe_int failed!");
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}

