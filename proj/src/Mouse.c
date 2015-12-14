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
	hook_id_mouse = MOUSE_IRQ;
	int bitmask = BIT(hook_id_mouse);
	if (sys_irqsetpolicy(MOUSE_IRQ, IRQ_REENABLE | IRQ_EXCLUSIVE,
			&hook_id_mouse) == OK && sys_irqenable(&hook_id_mouse) == OK)
		return bitmask;
	else
		return EXIT_FAILURE;
}

int unsubscribeMouse() {
	if (sys_irqrmpolicy(&hook_id_mouse) == OK
			&& sys_irqdisable(&hook_id_mouse) == OK)
		return EXIT_SUCCESS;
	else
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

void deleteMouse() {
	free(getMouse());
}

void drawMouse() {
	drawSquare(mouse->x, mouse->y, 10, COLOUR_BLUE);
	//mouse->draw = 0;
}
