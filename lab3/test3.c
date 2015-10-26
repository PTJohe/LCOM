#include <minix/syslib.h>
#include <minix/drivers.h>
#include <minix/com.h>
#include <stdio.h>
#include <stdlib.h>
#include "i8042.h"
#include "test3.h"
#include <minix/sysutil.h>
#include "keyboard.h"

static int hook_id_keyboard = 1;
static int timer_counter;
static int hook_id_timer;

int kbd_subscribe_int() {

	int bitmask = BIT(KBC_HOOK_BIT); /* hook_id_keyboard = 1 */
	if (sys_irqsetpolicy(KBC_IRQ, IRQ_REENABLE | IRQ_EXCLUSIVE, &hook_id_keyboard) == OK	&& sys_irqenable(&hook_id_keyboard) == OK)
		return bitmask;
	else
		return EXIT_FAILURE;
}

int kbd_unsubscribe_int() {

	if (sys_irqdisable(&hook_id_keyboard) == OK && sys_irqrmpolicy(&hook_id_keyboard) == OK)
		return EXIT_SUCCESS;
	else
		return EXIT_FAILURE;
}

unsigned long keyboard_int_handler() {
	unsigned long data;
	unsigned long stat;
	while (1) {
		sys_inb(KBC_STAT_REG, &stat);
		if (stat & KBC_OBF) {
			if (sys_inb(KBC_OUT_BUF, &data) == OK) {
				if ((stat & (PAR_ERR | TO_ERR)) == 0)
					return data;
			}
			else
				return EXIT_FAILURE;
		}
		tickdelay(micros_to_ticks(DELAY_US));
	}
}

int kbd_test_scan(unsigned short ass) {
	int irq_set = kbd_subscribe_int();
	int r;
	int ipc_status;
	int state = 0;
	unsigned long scancode = 0;
	message msg;
	//sys_outb(KBC_IN_BUF, 0xF4);

	while (1) { /* You may want to use a different condition */
		/* Get a request message. */
		if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) {
			printf("driver_receive failed with: %d", r);
			continue;
		}
		if (is_ipc_notify(ipc_status)) { /* received notification */
			switch (_ENDPOINT_P(msg.m_source)) {
			case HARDWARE: /* hardware interrupt notification */
				if (msg.NOTIFY_ARG & irq_set) {
					if (ass == 0) {
						scancode = keyboard_int_handler();
						if (scancode == MAKE_ESC_CODE) {
							printf("End of kbd_test_scan");
							return EXIT_SUCCESS;
						}
						if (scancode == 0xE0) {
							state = 1;
							break;
						}
						else{
						if (scancode & BIT(7))
							printf("Breakcode: ");
						else
							printf("Makecode: ");

						if (state == 1) {
							printf("0xE0%02X\n", scancode);
							state = 0;
							break;
						}
						else if (state == 0)
							printf("0x%02X\n", scancode);
						}
					}
				} else
					return EXIT_FAILURE;
				break; /* no other notifications expected: do nothing */
			default:
				break;
			}
		}
		else { /* received a standard message, not a notification */
			/* no standard messages expected: do nothing */
	}

}
	kbd_unsubscribe_int();
	printf("kbd_test_scan concluido.\n");
	return EXIT_SUCCESS;

}

unsigned long issue_data_KBC(unsigned char data, unsigned long stat) {
	stat = 0;
	if (sys_inb(KBC_STAT_REG, &stat) == OK) {
		if ((stat & KBC_IBF) == 0) {
			if (sys_outb(KBC_CMD_REG, data) == OK)
				return EXIT_SUCCESS;
			else
				return EXIT_FAILURE;
		}
		tickdelay(micros_to_ticks(DELAY_US));

	}
}

unsigned long issue_commandArgument_KBC(unsigned char command, unsigned long argument, unsigned long stat) {
	stat = 0;
	unsigned long verify;
	verify = issue_data_KBC(command, stat);
	if (verify == 1)
		return EXIT_FAILURE;
	if (sys_inb(KBC_OUT_BUF, &stat) == OK) {
		if (stat == ACK) {
			issue_data_KBC(argument, stat);
			if (sys_inb(KBC_OUT_BUF, &stat) == OK) {
				switch (stat) {
				case ACK:
					return EXIT_SUCCESS;
				case RESEND:
					sys_inb(KBC_OUT_BUF, &stat);
				case ERROR:
					return EXIT_FAILURE;
				}
			}

		}
	}
	else
		return EXIT_FAILURE;

}

int timer_subscribe_int(void) {
	hook_id_timer = 0;
	int bitmask = BIT(hook_id_timer);
	if (sys_irqsetpolicy(TIMER0_IRQ, IRQ_REENABLE, &hook_id_timer) == OK && sys_irqenable(&hook_id_timer) == OK)
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

int timer_interrupt(unsigned long time){
	if(time == 0){
			printf("Error: Time interval must be positive.\n");
			return EXIT_FAILURE;
		}

		timer_counter = 0; // initialize "counter" global variable
		int irq_set = timer_subscribe_int();
		int interrupt = BIT(irq_set);
		int r;
		int ipc_status;
		message msg;
		while( (time * 60) >= timer_counter ) { /* You may want to use a different condition */
			/* Get a request message. */
			if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0 ) {
					printf("driver_receive failed with: %d", r);
					continue;
				}
			if (is_ipc_notify(ipc_status)) { /* received notification */
				switch (_ENDPOINT_P(msg.m_source)) {
				case HARDWARE: /* hardware interrupt notification */
					if (msg.NOTIFY_ARG & interrupt) {
						timer_counter += 1;
					}
					break;
				default:
					break; /* no other notifications expected: do nothing */
					}
				}
			else { /* received a standard message, not a notification */
						/* no standard messages expected: do nothing */
			}
		}
		timer_unsubscribe_int();
		return EXIT_SUCCESS;
}

int kbd_test_leds(unsigned short n, unsigned short *leds) {
	int verify;
	unsigned long stat = 0;
	verify = kbd_subscribe_int();
	if (verify == 1)
		return EXIT_FAILURE;
	unsigned int l;
	unsigned short (*arg) = 0;
	for (l = 0; l < n; l++){
		if (leds[l] == 0 || leds[l] == 1 || leds[l] == 2)
			*arg ^= leds[l];
		printf("szfzdfg");
		issue_commandArgument_KBC(KBD_TOGGLE_LEDS, *arg, stat);
		timer_interrupt(1); // "The function kbd_test_leds() should process each element of the toggle array once per second"
		}
	kbd_unsubscribe_int();
}

int kbd_test_timed_scan(unsigned short n) {
/* To be completed */
}

