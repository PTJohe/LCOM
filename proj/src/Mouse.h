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

	int leftButton;
	int middleButton;
	int rightButton;

	int cursor;

	int hasBeenUpdated;
	int draw;
} Mouse;

int subscribeMouse();
int unsubscribeMouse();
unsigned short read_data_OUTBUF_from_KBC(unsigned long* data);
unsigned long mouse_int_handler();
unsigned short KBC_issue_command_mouse(unsigned char command);
int mouse_enable_stream_mode();
int mouse_disable_stream_mode();
unsigned short send_argument_mouse(unsigned char argument);
unsigned short issue_argument_KBC(unsigned char argument);
int mouse_enable_sending_packets();
Mouse* newMouse();
Mouse* getMouse();
void updateMouse();
void drawMouse();

#endif
