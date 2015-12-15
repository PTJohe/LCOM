#include "Mouse.h"
#include "Utilities.h"

#include <minix/sysutil.h>
#include <minix/syslib.h>
#include <minix/drivers.h>
#include <minix/com.h>
#include <stdio.h>
#include <stdlib.h>

//Global variable
Mouse* mouse = NULL;
Mouse mousePreviousState;

int subscribeMouse() {
	int byte_number = mouse->byteBeingRead;
	int bitmask = BIT(hook_id_mouse);
	if (sys_irqsetpolicy(MOUSE_IRQ, IRQ_REENABLE | IRQ_EXCLUSIVE,
			&hook_id_mouse) == OK && sys_irqenable(&hook_id_mouse) == OK) {
		memset(mouse->packet, 0, sizeof(mouse->packet));
		byte_number = 0; // set all bytes bit 3 to 0 and byte_number (index of byte in packet of 3 bytes) to 0
		return bitmask;
	} else
		return EXIT_FAILURE;
}

int unsubscribeMouse() {
	if (sys_irqdisable(&hook_id_mouse) == OK
			&& sys_irqrmpolicy(&hook_id_mouse) == OK)
		return EXIT_SUCCESS;
	else
		return EXIT_FAILURE;
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

unsigned long mouse_int_handler() {
	unsigned long stat;
	int byte_number = mouse->byteBeingRead;
	byte_number = 0;
	if (read_data_OUTBUF_from_KBC(&stat) != EXIT_FAILURE) {
		mouse->packet[byte_number] = stat;

		if (byte_number == 0 && (mouse->packet[0] & BIT(3) == 0))
			return EXIT_FAILURE;
		byte_number = (byte_number + 1) % 3;
	}
	return EXIT_SUCCESS;
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

int mouse_enable_stream_mode() {
	if (send_argument_mouse(MOUSE_STREAM_MODE) == EXIT_SUCCESS) {
		return EXIT_SUCCESS;
	} else
		return EXIT_FAILURE;
}

int mouse_disable_stream_mode() {
	int byte_number = mouse->byteBeingRead;
	if (send_argument_mouse(MOUSE_DISABLE_STREAM_MODE) == EXIT_SUCCESS) {
		memset(mouse->packet, 0, sizeof(mouse->packet));
		byte_number = 0; // set all bytes bit 3 to 0 and byte_number (index of byte in packet of 3 bytes) to 0
		return EXIT_SUCCESS;
	} else
		return EXIT_FAILURE;
}

unsigned short send_argument_mouse(unsigned char argument) {
	unsigned long KBC_value;
	if (KBC_issue_command_mouse(WRITE_BYTE_TO_MOUSE) == EXIT_FAILURE)
		return EXIT_FAILURE;
	if (issue_argument_KBC(argument) == EXIT_FAILURE)
		return EXIT_FAILURE;
	if (read_data_OUTBUF_from_KBC(&KBC_value) == EXIT_FAILURE)
		return EXIT_FAILURE;
	if (KBC_value == ACK)
		return EXIT_SUCCESS;
	else
		return EXIT_FAILURE;
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

int mouse_enable_sending_packets() {
	int byte_number = mouse->byteBeingRead;
	unsigned short verify;
	verify = send_argument_mouse(MOUSE_STREAM_MODE);
	if (verify == EXIT_SUCCESS) {
		if (send_argument_mouse(MOUSE_SEND_PACKETS) == EXIT_SUCCESS) {
			memset(mouse->packet, 0, sizeof(mouse->packet));
			byte_number = 0; // set all bytes bit 3 to 0 and byte_number (index of byte in packet of 3 bytes) to 0
			return EXIT_SUCCESS;
		}
	} else
		return EXIT_FAILURE;
}

Mouse* getMouse() {
	if (!mouse) {
		mouse = newMouse();
	}
	return mouse;
}

Mouse* newMouse() {
	Mouse* mouse = (Mouse*) malloc(sizeof(Mouse));

	mouse->x = 50;
	mouse->y = 50;
	mouse->sensitivity = 1.0;

	mouse->byteBeingRead = 0;

	mouse->leftButton = 0;
	mouse->rightButton = 0;
	mouse->middleButton = 0;

	mouse->cursor = 0;

	mouse->hasBeenUpdated = 0;
	mouse->draw = 1;

	return mouse;
}

void updateMouse() {
	int irq_set = subscribeMouse();
	if (irq_set == EXIT_FAILURE)
		return;
	if (KBC_issue_command_mouse(ENABLE_MOUSE) == EXIT_FAILURE)
		return;
	int stream_mode = mouse_enable_stream_mode();
	if (stream_mode == EXIT_FAILURE)
		return;
	int send_packets = mouse_enable_sending_packets();
	if (send_packets == EXIT_FAILURE)
		return;
	if (mouse_int_handler() == EXIT_FAILURE)
		return;

	int xSign = mouse->packet[0] & BIT(4);
	int ySign = mouse->packet[0] & BIT(5);
	int deltaX = mouse->packet[1];
	int deltaY = mouse->packet[2];
	if (xSign)
		deltaX |= (-1 << 8);
	if (ySign)
		deltaY |= (-1 << 8);

	mouse->x += mouse->sensitivity * deltaX;
	if (mouse->x < 0)
		mouse->x = 0;
	else if (mouse->x >= getHRes())
		mouse->x = getHRes() - 1;

	mouse->y += mouse->sensitivity * deltaY;
	if (mouse->y < 0)
		mouse->y = 0;
	else if (mouse->y >= getVRes())
		mouse->y = getVRes() - 1;

	mouse->hasBeenUpdated = 1;
	int disable_stream_mode = mouse_disable_stream_mode();
	if (disable_stream_mode == EXIT_FAILURE)
		return;
	if (unsubscribeMouse() == EXIT_FAILURE) {
		//printf("ERROR: mouse_unsubscribe_int failed!");
		return;
	}
}

void deleteMouse() {
	free(getMouse());
}

void drawMouse() {
	//updateMouse();
	drawSquare(mouse->x, mouse->y, 10, COLOUR_BLUE);
	//mouse->draw = 0;
}
