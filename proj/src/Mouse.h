#ifndef __MOUSE_H
#define __MOUSE_H

static int hook_id_mouse;

// Represents a mouse
typedef struct {
	int x, y;
	int xSign, ySign;
	int deltaX, deltaY;
	double sensitivity;

	int byteBeingRead;
	unsigned long packet[3];

	int leftButton;
	int middleButton;
	int rightButton;

	int cursor;

	int hasBeenUpdated;
	int draw;
} Mouse;

int subscribeMouse();
int unsubscribeMouse();
Mouse* newMouse();
Mouse* getMouse();
void drawMouse();

#endif
