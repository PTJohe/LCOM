#include <minix/drivers.h>
#include "WallyNIX.h"

#include "Utilities.h"
#include "Bitmap.h"
#include "Graphics.h"
#include "Keyboard.h"
#include "Mouse.h"
#include "Timer.h"
#include "State.h"

const int FPS = 30;
const int mouseFPS = 75;

WallyNIX* startWallyNIX() {
	WallyNIX* wally = (WallyNIX*) malloc(sizeof(WallyNIX));

	wally->menu = 0;

	wally->option = 0;
	wally->timeLimit = 10 * TIMER_DEFAULT_FREQ;
	wally->exit = 0, wally->draw = 1;
	wally->scancode = 0;

	// Subscribe devices
	wally->IRQ_SET_KBD = subscribeKeyboard();
	wally->IRQ_SET_TIMER = subscribeTimer();
	wally->IRQ_SET_MOUSE = subscribeMouse();

	wally->level1 = loadBitmap(getStagePath(1));

	wally->mainMenu = createMainMenu();
	wally->timer = createTimer();

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
			if (msg.NOTIFY_ARG & wally->IRQ_SET_TIMER) {
				if (wally->menu == 1) {
					if (wally->timer->enabled == 1)
						timerCount(wally->timer);
					if (wally->timer->counter == 0) {
						wally->menu = 0;
						wally->option = 0;
					}
				}
			}
			// Mouse interruption
			if (msg.NOTIFY_ARG & wally->IRQ_SET_MOUSE) {
				updateMouse();
			}
			break;
		default:
			break;
		}
	}
	if (!wally->mainMenu->done) {
		updateMainMenu(wally->mainMenu);
		switch (wally->mainMenu->mouseSelection) {
		case 1:
			wally->mainMenu->done = 1;
			wally->menu = 1;
			resetTimer(wally->timer);
			startTimer(wally->timer);
			break;
		case 2:
			wally->exit = 1;
			break;
		default:
			break;
		}
	}

	if (wally->scancode != 0) {
		if (wally->scancode == KEY_ESC) {
			if (wally->menu == 1) {
				wally->menu = 0;
				resetMainMenu(wally->mainMenu);
				wally->option = 0;
				wally->scancode = 0;
			} else
				wally->exit = 1;
		} else if (wally->scancode == KEY_W) {
			wally->scancode = 0;
			if (wally->option - 1 >= 0)
				wally->option -= 1;
		} else if (wally->scancode == KEY_S) {
			wally->scancode = 0;
			if (wally->option + 1 <= 1)
				wally->option += 1;
		} else if (wally->scancode == KEY_ENTER) {
			wally->scancode = 0;
			if (wally->menu == 0) {
				if (wally->option == 0) {
					wally->menu = 1;
					resetTimer(wally->timer);
					startTimer(wally->timer);
				} else
					wally->exit = 1;
			}
		}
	}
}

void drawWallyNIX(WallyNIX* wally) {
	if (wally->menu == 0) {
		drawMainMenu(wally->mainMenu);
	} else if (wally->menu == 1) {
		drawStage(wally->level1);
		drawTimeLeft(wally->timer);
	}
}

void stopWallyNIX(WallyNIX* wally) {
	deleteTimer(wally->timer);
	deleteMouse();

	deleteMainMenu(wally->mainMenu);

	//Unsubscribe devices
	unsubscribeKeyboard();
	unsubscribeTimer();
	unsubscribeMouse();

	free(wally);
}

