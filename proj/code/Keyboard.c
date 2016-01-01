#include "Keyboard.h"
#include "Utilities.h"

#include <minix/syslib.h>
#include <minix/drivers.h>
#include <minix/com.h>
#include <stdio.h>
#include <stdlib.h>

int subscribeKeyboard() {
	hook_id_keyboard = KBC_IRQ;
	int bitmask = BIT(hook_id_keyboard); //KBC_IRQ = hook_id_keyboard = 1
	if (sys_irqsetpolicy(KBC_IRQ, IRQ_REENABLE | IRQ_EXCLUSIVE,
			&hook_id_keyboard) == OK && sys_irqenable(&hook_id_keyboard) == OK)
		return bitmask;
	else
		return EXIT_FAILURE;
}

int unsubscribeKeyboard() {
	if (sys_irqdisable(&hook_id_keyboard) != OK
			&& sys_irqrmpolicy(&hook_id_keyboard) != OK)
		return EXIT_FAILURE;
	else
		return EXIT_SUCCESS;
}

unsigned short read_data_OUTBUF_from_KBC(unsigned long* data) {
	unsigned long status_register;
	while (1) {
		if (sys_inb(KBC_STAT_REG, &status_register) != OK)
			return EXIT_FAILURE;
		if (status_register & KBC_OBF) {
			if (sys_inb(KBC_OUT_BUF, data) == OK) {
				if ((status_register & (PAR_ERR | TO_ERR)) == 0)
					return EXIT_SUCCESS;
			} else
				return EXIT_FAILURE;
		}
		tickdelay(micros_to_ticks(DELAY_US));
	}

}

unsigned short KBC_issue_command_mouse(unsigned char command) {
	unsigned long stat = 0;
	if (sys_inb(KBC_STAT_REG, &stat) == OK) {
		if ((stat & KBC_IBF) == 0) {
			if (sys_outb(KBC_CMD_REG, command) == OK)
				return EXIT_SUCCESS;
			else
				return EXIT_FAILURE;
		}
		tickdelay(micros_to_ticks(DELAY_US));

	}
}

unsigned short issue_argument_KBC(unsigned char argument) {
	unsigned long stat = 0;
	if (sys_inb(KBC_STAT_REG, &stat) == OK) {
		if ((stat & KBC_IBF) == 0) {
			if (sys_outb(KBC_IN_BUF, argument) == OK)
				return EXIT_SUCCESS;
			else
				return EXIT_FAILURE;
		}
		tickdelay(micros_to_ticks(DELAY_US));
	}

}

unsigned long readScancode() {
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
