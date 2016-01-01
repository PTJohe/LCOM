#ifndef __MOUSE_H
#define __MOUSE_H

#include "Position.h"
#include "Bitmap.h"
#include <minix/sysutil.h>

static int hook_id_mouse = 12;

// Represents a mouse
typedef struct {
	int x, y;
	Position* position;
	int xSign, ySign;
	int deltaX, deltaY;
	double sensitivity;

	int byteNumber;
	unsigned long packet[3];

	int leftButtonPressed;
	int leftButtonReleased;

	Bitmap* cursor;

	int hasBeenUpdated;
	int draw;
} Mouse;

int subscribeMouse();
int unsubscribeMouse();
int enableMouse();
int readMouse(unsigned long* reg);
int writeToKBC(unsigned long port, unsigned char byte);
int writeToMouse(unsigned char byte);
Mouse* getMouse();
Mouse* createMouse();
void updateMouse();
void resetMouseButton();
void drawMouse();
void deleteMouse();

#endif
