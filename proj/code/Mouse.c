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

int subscribeMouse() {
	int bitmask = BIT(hook_id_mouse);
	if (sys_irqsetpolicy(MOUSE_IRQ, IRQ_REENABLE | IRQ_EXCLUSIVE,
			&hook_id_mouse) == OK && sys_irqenable(&hook_id_mouse) == OK) {
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

int enableMouse() {
	unsigned long result;

	do {
		writeToMouse(0xF4);
		readMouse(&result);
	} while (result != ACK);

	return 0;
}

int readMouse(unsigned long* reg) {
	return (sys_inb(KBC_DATA_REG, reg) != OK);
}

int writeToKBC(unsigned long port, unsigned char byte) {
	unsigned long stat;

	while (1) {
		if (sys_inb(KBC_STAT_REG, &stat) != OK)
			return -1;
		if ((stat & KBC_IBF) == 0) {
			if (sys_outb(port, byte) != OK)
				return -1;
			break;
		}
		tickdelay(micros_to_ticks(DELAY_US));
	}
	return 0;
}

int writeToMouse(unsigned char byte) {
	if (writeToKBC(KBC_STAT_REG, 0xD4) != OK)
		return 1;
	if (writeToKBC(KBC_DATA_REG, byte) != OK)
		return 1;
	return 0;
}

Mouse* getMouse() {
	if (!mouse) {
		enableMouse();
		mouse = createMouse();
	}
	return mouse;
}

Mouse* createMouse() {
	Mouse* mouse = (Mouse*) malloc(sizeof(Mouse));

	mouse->position = createPosition(0, 0);

	mouse->leftButtonPressed = 0;
	mouse->leftButtonReleased = 0;

	mouse->cursor = loadBitmap(getMousePath(2));
	mouse->sensitivity = 1.0;

	mouse->byteNumber = 0;
	mouse->draw = 1;
	mouse->hasBeenUpdated = 0;

	return mouse;
}

void updateMouse() {
	Mouse* mouse = getMouse();

	readMouse(&mouse->packet[mouse->byteNumber]);

	if (mouse->byteNumber == 0 && !(mouse->packet[mouse->byteNumber] & BIT(3)))
		return;
	mouse->byteNumber++;

	if (mouse->byteNumber > 2) {
		mouse->byteNumber = 0;

		int previousX = mouse->position->x;
		int previousY = mouse->position->y;
		int previousPressed = mouse->leftButtonPressed;

		mouse->xSign = mouse->packet[0] & X_SIGN;
		mouse->ySign = mouse->packet[0] & Y_SIGN;
		mouse->deltaX = mouse->packet[1];
		mouse->deltaY = mouse->packet[2];

		if (mouse->xSign) {
			mouse->deltaX = mouse->deltaX | (-1 << 8);
		}
		if (mouse->ySign) {
			mouse->deltaY = mouse->deltaY | (-1 << 8);
		}

		mouse->leftButtonPressed = mouse->packet[0] & LEFT_BUTTON;
		if (mouse->leftButtonPressed == 0 && previousPressed != 0) {
			mouse->leftButtonReleased = 1;
		} else
			mouse->leftButtonReleased = 0;

		if (mouse->deltaX != 0) {
			if (mouse->position->x + mouse->deltaX * mouse->sensitivity < 0)
				mouse->position->x = 0;
			else if (mouse->position->x + mouse->deltaX * mouse->sensitivity
					>= getHRes())
				mouse->position->x = getHRes() - 1;
			else
				mouse->position->x += mouse->deltaX * mouse->sensitivity;
		}
		if (mouse->deltaY != 0) {
			if (mouse->position->y - mouse->deltaY * mouse->sensitivity < 0)
				mouse->position->y = 0;
			else if (mouse->position->y - mouse->deltaY * mouse->sensitivity
					>= getVRes())
				mouse->position->y = getVRes() - 1;
			else
				mouse->position->y -= mouse->deltaY * mouse->sensitivity;
		}

		mouse->deltaX = mouse->position->x - previousX;
		mouse->deltaY = mouse->position->y - previousY;

		mouse->hasBeenUpdated = 1;
	}
}

void resetMouseButton() {
	getMouse()->leftButtonPressed = 0;
	getMouse()->leftButtonReleased = 0;
}

void drawMouse() {
	drawBitmapAlpha(mouse->cursor, mouse->position->x, mouse->position->y,
	COLOUR_WHITE);
}

void deleteMouse() {
	free(getMouse());
}
