#include "Mouse.h"
#include "Utilities.h"
#include "Keyboard.h"
#include "Graphics.h"

#include <minix/sysutil.h>
#include <minix/syslib.h>
#include <minix/drivers.h>
#include <minix/com.h>
#include <stdio.h>
#include <stdlib.h>

//Global variable
Mouse* mouse = NULL;
Mouse mousePreviousFlags;

//Mouse* newMouse();

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

/*unsigned long mouse_int_handler() {
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
}*/

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

unsigned long enableMouse(){
	if (mouse_enable_sending_packets() == EXIT_FAILURE)
		return EXIT_FAILURE;
	else
		return EXIT_SUCCESS;
}

Mouse* getMouse() {
	if (!mouse) {
		enableMouse();
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

	mouse->leftButtonPressed = 0;
	mouse->rightButtonPressed = 0;
	mouse->middleButtonPressed = 0;

	mouse->leftButtonReleased = 0;
	mouse->rightButtonReleased = 0;
	mouse->middleButtonReleased = 0;

	mouse->cursor = 0;

	mouse->hasBeenUpdated = 0;
	mouse->draw = 1;

	mousePreviousFlags = *mouse;

	return mouse;
}

void incrementPacketBytes(){
	mouse->byteBeingRead++;
}

void resetCounterPacketBytes(){
	mouse->byteBeingRead = 0;
}

void updateMouse() {
	Mouse* mouse = getMouse();
	read_data_OUTBUF_from_KBC(&mouse->packet[mouse->byteBeingRead]);
	if (!(mouse->packet[mouse->byteBeingRead] & BIT(3)) && mouse->byteBeingRead) // synchronize packet bytes
		return;

	incrementPacketBytes();

	if (mouse->byteBeingRead > 2){
		resetCounterPacketBytes();
		mouse->xSign = mouse->packet[0] & X_SIGN;
		mouse->ySign = mouse->packet[0] & Y_SIGN;
		mouse->deltaX = mouse->packet[1];
		mouse->deltaY = mouse->packet[2];

		if (mouse->xSign == 1){
			mouse->deltaX = mouse->deltaX | (-1 << 8);
		}
		if (mouse->ySign == 1){
			mouse->deltaY = mouse->deltaY | (-1 << 8);
		}

		mouse->leftButtonPressed = mouse->packet[0] & LEFT_BUTTON;
		mouse->rightButtonPressed = mouse->packet[0] & RIGHT_BUTTON;
		mouse->middleButtonPressed = mouse->packet[0] & MIDDLE_BUTTON;

		if (mouse->leftButtonPressed == 0 && mousePreviousFlags.leftButtonPressed != 0){
			mouse->leftButtonReleased = 1;
		}
		if (mouse->rightButtonPressed != 0 && mousePreviousFlags.rightButtonPressed == 0){
			mouse->rightButtonReleased = 1;
		}
		if (mouse->middleButtonPressed != 0 && mousePreviousFlags.middleButtonPressed == 0){
			mouse->middleButtonReleased = 1;
		}

		if (mouse->deltaX != 0){
			mouse->x = mouse->x + (mouse->deltaX * mouse->sensitivity);
			if (mouse->x >= getHRes())
				mouse->x = getHRes() - 1;
			else if (mouse->x < 0)
				mouse->x = 0;
		}

		if (mouse->deltaY != 0){
			mouse->y = mouse->y + (mouse->deltaY * mouse->sensitivity);
			if (mouse->y >= getVRes())
				mouse->y = getVRes() - 1;
			else if (mouse->y < 0)
				mouse->y = 0;
		}

		mouse->deltaX = mouse->x - mousePreviousFlags.x;
		mouse->deltaY = mouse->y - mousePreviousFlags.y;

		mousePreviousFlags = *mouse;
		mouse->hasBeenUpdated = 1;

	}
	/*int irq_set = subscribeMouse();
	if (irq_set == EXIT_FAILURE)
		return;*/
	/*if (KBC_issue_command_mouse(ENABLE_MOUSE) == EXIT_FAILURE)
		return;
	int stream_mode = mouse_enable_stream_mode();
	if (stream_mode == EXIT_FAILURE)
		return;
	int send_packets = mouse_enable_sending_packets();
	if (send_packets == EXIT_FAILURE)
		return;*/
	/*if (mouse_int_handler() == EXIT_FAILURE)
		return;*/

	/*int disable_stream_mode = mouse_disable_stream_mode();
	if (disable_stream_mode == EXIT_FAILURE)
		return;*/
	/*if (unsubscribeMouse() == EXIT_FAILURE) {
		//printf("ERROR: mouse_unsubscribe_int failed!");
		return;
	}*/
	LOG_VAR("mouse x:", mouse->x);
	LOG_VAR("mouse y:", mouse->y);
}

void deleteMouse() {
	//mouse_disable_stream_mode();
	free(getMouse());
}

void drawMouse() {
	//updateMouse();
	drawSquare(mouse->x, mouse->y, 10, COLOUR_BLUE);
	getMouse()->draw = 0;
}
