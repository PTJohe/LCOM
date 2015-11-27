#ifndef __WALLYNIX_H
#define __WALLYNIX_H

#include "Timer.h"

typedef struct {
	unsigned short mode;

	int IRQ_SET_TIMER;
	int IRQ_SET_KBD;
	int IRQ_SET_MOUSE;

	int menu, done, draw;
	unsigned long scancode;
	Timer* timer;
} WallyNIX;

WallyNIX* initWallyNIX();
void updateWallyNIX(WallyNIX* wally);
void drawWallyNIX(WallyNIX* wally);
void stopWallyNIX(WallyNIX* wally);

#endif
