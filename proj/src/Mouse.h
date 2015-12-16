#ifndef __MOUSE_H
#define __MOUSE_H

#include <minix/sysutil.h>

static int hook_id_mouse;

// Represents a mouse
typedef struct {
	int x, y;
	int xSign, ySign;
	int deltaX, deltaY;
	double sensitivity;

	int byteBeingRead;
	unsigned long packet[3];

	int leftButtonPressed;
	int middleButtonPressed;
	int rightButtonPressed;

	int leftButtonReleased;
	int middleButtonReleased;
	int rightButtonReleased;

	int cursor;

	int hasBeenUpdated;
	int draw;
} Mouse;

int subscribeMouse();
int unsubscribeMouse();
unsigned long mouse_int_handler();
int mouse_enable_stream_mode();
int mouse_disable_stream_mode();
unsigned short send_argument_mouse(unsigned char argument);
int mouse_enable_sending_packets();
unsigned long enableMouse();
Mouse* getMouse();
Mouse* newMouse();
void incrementPacketBytes();
void resetCounterPacketBytes();
void updateMouse();
void deleteMouse();
void drawMouse();

#endif
