#include <minix/drivers.h>
#include "WallyNIX.h"

#include "Utilities.h"
#include "Bitmap.h"
#include "Game.h"
#include "Graphics.h"
#include "Keyboard.h"
#include "Mouse.h"
#include "Timer.h"

const int FPS = 60;
const int mouseFPS = 75;

WallyNIX* startWallyNIX() {
	WallyNIX* wally = (WallyNIX*) malloc(sizeof(WallyNIX));

	wally->menu = MAIN_MENU;

	wally->option = 0;
	wally->exit = 0, wally->draw = 1;
	wally->scancode = 0;

	// Subscribe devices
	wally->IRQ_SET_KBD = subscribeKeyboard();
	wally->IRQ_SET_TIMER = subscribeTimer();
	wally->IRQ_SET_MOUSE = subscribeMouse();

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
			if (msg.NOTIFY_ARG & wally->IRQ_SET_KBD) {
				wally->scancode = readScancode();
				break;
			}
			// Timer interruption
			if (msg.NOTIFY_ARG & wally->IRQ_SET_TIMER) {
				if (wally->menu == ARCADE_MODE) {
					ArcadeMode* arcadeMode = wally->arcadeMode;
					if (arcadeMode->timer->enabled == 1)
						timerCount(arcadeMode->timer);
					if (arcadeMode->timer->counter >= 60 * TIMER_DEFAULT_FREQ) {
						arcadeMode->gameOver = 1;
						stopTimer(arcadeMode->timer);
					}
				}
			}
			// Mouse interruption
			if (msg.NOTIFY_ARG & wally->IRQ_SET_MOUSE) {
				getMouse()->draw = 1;
				updateMouse();
			}
			break;
		default:
			break;
		}
	}

	switch (wally->menu) {
	case MAIN_MENU:
		updateMainMenu(wally->mainMenu);

		switch (wally->mainMenu->mouseSelection) {
		case 1:
			deleteMainMenu(wally->mainMenu);
			wally->menu = ARCADE_MODE;
			wally->arcadeMode = createArcadeMode();
			startTimer(wally->arcadeMode->timer);
			break;
		case 5:
			wally->exit = 1;
			break;
		default:
			break;
		}
		break;
	case ARCADE_MODE:
		updateArcadeMode(wally->arcadeMode);

		if (wally->arcadeMode->done) {
			deleteArcadeMode(wally->arcadeMode);
			wally->menu = MAIN_MENU;
			wally->mainMenu = createMainMenu();
		}
		break;
	default:
		break;
	}

	switch (wally->scancode) {
	case KEY_ESC:
		wally->scancode = 0;
		if (wally->menu == ARCADE_MODE) {
			deleteArcadeMode(wally->arcadeMode);
			wally->menu = MAIN_MENU;
			wally->mainMenu = createMainMenu();
			wally->option = 0;
		} else
			wally->exit = 1;
		break;
	case KEY_W:
		wally->scancode = 0;

		if (wally->menu == MAIN_MENU) {
			getMouse()->draw = 0;
			if (wally->mainMenu->option - 1 >= 0)
				wally->mainMenu->option -= 1;
		}
		break;
	case KEY_S:
		wally->scancode = 0;

		if (wally->menu == MAIN_MENU) {
			getMouse()->draw = 0;
			if (wally->mainMenu->option + 1 <= 4)
				wally->mainMenu->option += 1;
		}
		break;
	case KEY_ENTER:
		wally->scancode = 0;
		if (wally->menu == MAIN_MENU) {
			if (wally->mainMenu->option == 0) {
				deleteMainMenu(wally->mainMenu);
				wally->menu = ARCADE_MODE;
				wally->arcadeMode = createArcadeMode();
				startTimer(wally->arcadeMode->timer);
			} else if (wally->mainMenu->option == 4)
				wally->exit = 1;
		}
		break;
	default:
		break;
	}
}

void drawWallyNIX(WallyNIX* wally) {
	switch (wally->menu) {
	case MAIN_MENU:
		drawMainMenu(wally->mainMenu);
		break;
	case ARCADE_MODE:
		drawArcadeMode(wally->arcadeMode);
		break;
	default:
		break;
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

