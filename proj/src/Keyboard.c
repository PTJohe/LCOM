#include "Keyboard.h"
#include "Utilities.h"

#include <minix/syslib.h>
#include <minix/drivers.h>
#include <minix/com.h>
#include <stdio.h>
#include <stdlib.h>

int kbd_subscribe_int() {
	hook_id_keyboard = KBC_IRQ;
	int bitmask = BIT(hook_id_keyboard); //KBC_IRQ = hook_id_keyboard = 1
	if (sys_irqsetpolicy(KBC_IRQ, IRQ_REENABLE | IRQ_EXCLUSIVE,
			&hook_id_keyboard) == OK && sys_irqenable(&hook_id_keyboard) == OK)
		return bitmask;
	else
		return EXIT_FAILURE;
}

int kbd_unsubscribe_int() {
	if (sys_irqdisable(&hook_id_keyboard) != OK
			&& sys_irqrmpolicy(&hook_id_keyboard) != OK)
		return EXIT_FAILURE;
	else
		return EXIT_SUCCESS;
}

unsigned long kbd_read_scancode() {
	unsigned long data;
	unsigned long stat;
	while (1) {
		if (sys_inb(KBC_STAT_REG, &stat) != OK)
			return EXIT_FAILURE;
		if (stat & KBC_OBF) {
			if (sys_inb(KBC_OUT_BUF, &data) == OK) {
				if ((stat & (PAR_ERR | TO_ERR)) == 0)
					return data;
			} else
				return EXIT_FAILURE;
		}
		tickdelay(micros_to_ticks(DELAY_US));
	}
}
