#include "timer.h"
#include "constants.h"

#include <minix/syslib.h>
#include <minix/drivers.h>
#include <minix/com.h>
#include <stdio.h>
#include <stdlib.h>

int timer_subscribe_int(void) {
	hook_id_timer = TIMER0_IRQ;
	int bitmask = BIT(hook_id_timer); //TIMER0_IRQ = hook_id_timer = 0
	if (sys_irqsetpolicy(TIMER0_IRQ, IRQ_REENABLE, &hook_id_timer) == OK
			&& sys_irqenable(&hook_id_timer) == OK)
		return bitmask;
	else
		return EXIT_FAILURE;
}

int timer_unsubscribe_int() {
	if (sys_irqrmpolicy(&hook_id_timer) != OK
			&& sys_irqdisable(&hook_id_timer) != OK)
		return EXIT_FAILURE;
	else
		return EXIT_SUCCESS;
}

void timer_int_handler() {
	timer_counter = timer_counter + 1;
}

