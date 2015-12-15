#ifndef __WALLYNIX_H
#define __WALLYNIX_H

#include "Bitmap.h"
#include "Timer.h"

typedef struct {
	unsigned short mode;

	int IRQ_SET_TIMER;
	int IRQ_SET_KBD;
	int IRQ_SET_MOUSE;

	int menu, option, exit, draw;
	unsigned long scancode;
	Timer* timer;

	Bitmap* level1;

	Bitmap* char0;
	Bitmap* char1;
	Bitmap* char2;
	Bitmap* char3;
	Bitmap* char4;
	Bitmap* char5;
	Bitmap* char6;
	Bitmap* char7;
	Bitmap* char8;
	Bitmap* char9;

} WallyNIX;

WallyNIX* initWallyNIX();
void updateWallyNIX(WallyNIX* wally);
void drawWallyNIX(WallyNIX* wally);
void stopWallyNIX(WallyNIX* wally);

#endif
