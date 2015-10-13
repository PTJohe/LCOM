#include <minix/syslib.h>
#include <minix/drivers.h>
#include <minix/com.h>
#include <stdio.h>
#include <stdlib.h>
#include "i8254.h"
#include "timer.h"

static int hook_id;
static int counter;

int timer_set_square(unsigned long timer, unsigned long freq) {
	char buf[12];
	sprintf(buf, "TIMER_SEL", timer);
	unsigned long div = (TIMER_FREQ / freq);
	unsigned long LSB = div;
	unsigned long MSB = (div >> 8);
	unsigned char conf, port;
	if (freq < 19){
		printf("Error: Frequency must be greater than 18 Hz.\n");
		printf("OVERFLOW -> (Clock signal frequency)/freq must have a maximum of 16 bits.\n");
		return EXIT_FAILURE;
	}
	else if (TIMER_FREQ < freq){
		printf("Error: Frequency must be less than 1193182 Hz (clock signal frequency).\n");
		return EXIT_FAILURE;
	}
	int i = timer_get_conf(timer, &conf);
	if (i != 0)
		return EXIT_FAILURE;
	port = (TIMER_0 + timer);
	unsigned char counting_mode;
	if (conf == 0)
		counting_mode = TIMER_BIN;
	else if (conf == 1)
		counting_mode = TIMER_BCD;
	if (sys_outb(TIMER_CTRL, (int) buf | TIMER_LSB_MSB | TIMER_SQR_WAVE | counting_mode ) != OK
				|| sys_outb(port, LSB) != OK
				|| sys_outb(port, MSB) != OK)
		return EXIT_FAILURE;
	else
		return EXIT_SUCCESS;

}

int timer_subscribe_int(void) {

	hook_id = 0;
	if (sys_irqsetpolicy(TIMER0_IRQ, IRQ_REENABLE, &hook_id) == OK && sys_irqenable(&hook_id) == OK)
		return EXIT_SUCCESS;
	else
		return EXIT_FAILURE;
}

int timer_unsubscribe_int() {

	if (sys_irqrmpolicy(&hook_id) != OK)
		return EXIT_FAILURE;
	else
		return EXIT_SUCCESS;
}

void timer_int_handler() {
	counter = counter + 1;
}

int timer_get_conf(unsigned long timer, unsigned char *st) {

	char buf[12];
	sprintf(buf, "TIMER_SEL", timer);
	unsigned long cmd = (TIMER_RB_CMD | (int) buf | TIMER_RB_COUNT_);
	if (timer > 2 || timer < 0) {
		printf("Error: Timer value should be 0, 1 or 2.\n");
		return EXIT_FAILURE;
	} else {
		sys_outb(TIMER_CTRL, cmd);
		sys_inb(TIMER_0 + timer, (unsigned long*) st);
		return 0;
	}
}

int timer_display_conf(unsigned char conf) {
	printf("Output: ");
	if (conf & BIT(7))
		printf("True\n");
	else
		printf("False\n");
	printf("Null Count: ");
	if (conf & BIT(6))
		printf("True\n");
	else
		printf("False\n");
	printf("Type of Access: ");
	if (conf & (TIMER_LSB_MSB) == (TIMER_LSB_MSB))
		printf("LSB followed by MSB\n");
	else if (conf & (TIMER_LSB_MSB) == TIMER_LSB)
		printf("LSB\n");
	else
		printf("MSB\n");

	printf("Operating Mode ");
	unsigned long op_mode = conf >> 1 & (BIT(2) | BIT (1) | BIT(0));
	if (op_mode == 0)
		printf("0: INTERRUPT ON TERMINAL COUNT\n");
	else if (op_mode == 1)
		printf("1: HARDWARE RETRIGGERABLE ONE-SHOT\n");
	else if (op_mode == 2 || op_mode == 6){
		printf("2: RATE GENERATOR\n");
	}
	else if (op_mode = 3 || op_mode == 7)
			printf("3: SQUARE WAVE MODE\n");
	else if (op_mode == 4)
		printf("4: SOFTWARE TRIGGERED STROBE\n");
	else if (op_mode == 5)
		printf("5: HARDWARE TRIGGERED STROBE (RETRIGGERABLE)\n");

	printf("Counting Mode: ");
	if (conf & TIMER_BCD)
		printf("BCD\n");
	else
		printf("Binary\n");

	return EXIT_SUCCESS;
}

int timer_test_square(unsigned long freq) {
	timer_set_square(0, freq);
	return 0;
}

int timer_test_int(unsigned long time) {
	if(time == 0){
		printf("Error: Time interval must be positive.\n");
		return EXIT_FAILURE;
	}
	counter = 0; // initialize "counter" global variable
	int irq_set = timer_subscribe_int();
	int interrupt = BIT(irq_set);
	int r;
	int ipc_status;
	message msg;
	while( (time * 60) >= counter ) { /* You may want to use a different condition */
		/* Get a request message. */
		if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0 ) {
				printf("driver_receive failed with: %d", r);
				continue;
			}
		if (is_ipc_notify(ipc_status)) { /* received notification */
			switch (_ENDPOINT_P(msg.m_source)) {
			case HARDWARE: /* hardware interrupt notification */
				if (msg.NOTIFY_ARG & interrupt) {
					timer_int_handler();
						if (counter % 60 == 0)
							printf("1 segundo decorrido, Interrupcoes totais (60/segundo): %d \n", counter);
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
	printf("Timer_test_int concluido.\n");
	return EXIT_SUCCESS;
}

int timer_test_config(unsigned long timer) {
	unsigned char conf;
	timer_get_conf(timer, &conf);
	timer_display_conf(conf);
	if (conf = 0xB6)
		return EXIT_SUCCESS;
	else
		return EXIT_FAILURE;
}
