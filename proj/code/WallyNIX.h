#ifndef __WALLYNIX_H
#define __WALLYNIX_H

#include "Bitmap.h"
#include "Game.h"
#include "Timer.h"

typedef struct WallyNIX {
	int IRQ_SET_TIMER;
	int IRQ_SET_KBD;
	int IRQ_SET_MOUSE;

	int menu;
	int exit;

	unsigned long scancode;
	Timer* timerFPS;

	MainMenu* mainMenu;
	ArcadeMode* arcadeMode;
	StageSelect* stageSelect;
	HighScores* highScores;
	Options* options;

} WallyNIX;

WallyNIX* initWallyNIX();
void updateWallyNIX(WallyNIX* wally);
void updateInterruptions(WallyNIX* wally);
void updateKeyboard(WallyNIX* wally);
void updateStates(WallyNIX* wally);
void drawWallyNIX(WallyNIX* wally);
void stopWallyNIX(WallyNIX* wally);

#endif
