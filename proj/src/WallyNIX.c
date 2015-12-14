#include <minix/drivers.h>
#include "WallyNIX.h"

#include "Utilities.h"
#include "Bitmap.h"
#include "Graphics.h"
#include "Keyboard.h"
#include "Mouse.h"
#include "Timer.h"

const int FPS = 30;
const int mouseFPS = 75;

WallyNIX* startWallyNIX() {
	WallyNIX* wally = (WallyNIX*) malloc(sizeof(WallyNIX));

	wally->mode = MODE_1024_768;
	wally->menu = 0;
	wally->exit = 0, wally->draw = 1;
	wally->scancode = 0;
	wally->timer = newTimer();

	// Subscribe devices
	wally->IRQ_SET_TIMER = subscribeTimer();
	wally->IRQ_SET_KBD = subscribeKeyboard();

	wally->level1 = loadBitmap("/home/lcom1516-t2g15/proj/res/images/01.bmp");
	initGraphics(wally->mode);

	return wally;
}

void updateWallyNIX(WallyNIX* wally) {
	int ipc_status, r = 0;
	message msg;

	if (driver_receive(ANY, &msg, &ipc_status) != 0)
		return;

	if (is_ipc_notify(ipc_status)) {
		switch (_ENDPOINT_P(msg.m_source)) {
		case HARDWARE:
			// Keyboard interruption
			if (msg.NOTIFY_ARG & wally->IRQ_SET_KBD)
				wally->scancode = readScancode();
			// Timer interruption
			if (msg.NOTIFY_ARG & wally->IRQ_SET_TIMER)
				timerCount(wally->timer);
			break;
		default:
			break;
		}
	}

	if (wally->scancode != 0) {
		if (wally->scancode == KEY_ESC)
			wally->exit = 1;
		else if (wally->scancode == KEY_W) {
			wally->scancode = 0;
			wally->mode = 0x107;
			initGraphics(wally->mode);
		} else if (wally->scancode == KEY_A) {
			wally->scancode = 0;
			wally->mode = 0x101;
			initGraphics(wally->mode);
		} else if (wally->scancode == KEY_S) {
			wally->scancode = 0;
			wally->mode = 0x103;
			initGraphics(wally->mode);
		} else if (wally->scancode == KEY_D) {
			wally->scancode = 0;
			wally->mode = 0x105;
			initGraphics(wally->mode);
		}
	}
}

void drawWallyNIX(WallyNIX* wally) {
	fillDisplay(COLOUR_WHITE);
	drawBitmap(wally->level1, 0, 0, ALIGN_LEFT);
}

void stopWallyNIX(WallyNIX* wally) {
	//Unsubscrive devices
	unsubscribeTimer();
	unsubscribeKeyboard();

	deleteTimer(wally->timer);
	free(wally);

	exitGraphics();
}

