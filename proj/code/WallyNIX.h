#ifndef __WALLYNIX_H
#define __WALLYNIX_H

#include "Bitmap.h"
#include "Game.h"
#include "Timer.h"

typedef struct WallyNIX {
	int IRQ_SET_TIMER;
	int IRQ_SET_KBD;
	int IRQ_SET_MOUSE;

	int menu, option, exit, draw;

	unsigned long scancode;
	Timer* timer;

	MainMenu* mainMenu;
	ArcadeMode* arcadeMode;
} WallyNIX;

WallyNIX* initWallyNIX();
void updateWallyNIX(WallyNIX* wally);
void drawWallyNIX(WallyNIX* wally);
void stopWallyNIX(WallyNIX* wally);

#endif
