#ifndef __MOUSE_H
#define __MOUSE_H

#include "Bitmap.h"
#include <minix/sysutil.h>

static int hook_id_mouse = 12;

// Represents a mouse
typedef struct {
	int x, y;
	int xSign, ySign;
	int deltaX, deltaY;
	double sensitivity;

	int byteNumber;
	unsigned long packet[3];

	int leftButtonPressed;
	int middleButtonPressed;
	int rightButtonPressed;

	int leftButtonReleased;
	int middleButtonReleased;
	int rightButtonReleased;

	Bitmap* cursor;

	int hasBeenUpdated;
	int draw;
} Mouse;

int subscribeMouse();
int unsubscribeMouse();
int enableMouse();
Mouse* getMouse();
Mouse* createMouse();
void deleteMouse();
void drawMouse();
void updateMouse();
int readMouse(unsigned long* reg);
int writeToKBC(unsigned long port, unsigned char byte);
int writeToMouse(unsigned char byte);

#endif
