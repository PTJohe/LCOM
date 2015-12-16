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

	wally->mode = MODE_1280_1024;
	wally->menu = 0;
	wally->option = 0;
	wally->timeLimit = 10*60;
	wally->exit = 0, wally->draw = 1;
	wally->scancode = 0;

	wally->timer = newTimer();

	// Subscribe devices
	wally->IRQ_SET_TIMER = subscribeTimer();
	wally->IRQ_SET_KBD = subscribeKeyboard();
	wally->IRQ_SET_MOUSE = subscribeMouse();

	wally->level1 = loadBitmap(
			"/home/lcom/lcom1516-t2g15/proj/res/images/01.bmp");

	wally->char0 = loadBitmap("/home/lcom/lcom1516-t2g15/proj/res/font/0.bmp");
	wally->char1 = loadBitmap("/home/lcom/lcom1516-t2g15/proj/res/font/1.bmp");
	wally->char2 = loadBitmap("/home/lcom/lcom1516-t2g15/proj/res/font/2.bmp");
	wally->char3 = loadBitmap("/home/lcom/lcom1516-t2g15/proj/res/font/3.bmp");
	wally->char4 = loadBitmap("/home/lcom/lcom1516-t2g15/proj/res/font/4.bmp");
	wally->char5 = loadBitmap("/home/lcom/lcom1516-t2g15/proj/res/font/5.bmp");
	wally->char6 = loadBitmap("/home/lcom/lcom1516-t2g15/proj/res/font/6.bmp");
	wally->char7 = loadBitmap("/home/lcom/lcom1516-t2g15/proj/res/font/7.bmp");
	wally->char8 = loadBitmap("/home/lcom/lcom1516-t2g15/proj/res/font/8.bmp");
	wally->char9 = loadBitmap("/home/lcom/lcom1516-t2g15/proj/res/font/9.bmp");

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
			if (msg.NOTIFY_ARG & wally->IRQ_SET_TIMER) {
				timerCount(wally->timer);
				if (wally->menu == 1) {
					wally->timeLimit -= 1;
					if (wally->timeLimit == 0) {
						wally->menu = 0;
						wally->option = 0;
					}
				}
			}
			// Mouse interruption
			if (msg.NOTIFY_ARG & wally->IRQ_SET_MOUSE){
				updateMouse();
			}
			break;
		default:
			break;
		}
	}

	/*if (wally->timer->enabled)
		getMouse()->draw = 1;*/
	if (wally->scancode != 0) {
		if (wally->scancode == KEY_ESC) {
			if (wally->menu == 1) {
				wally->menu = 0;
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
			if (wally->option + 1 >= 1)
				wally->option += 1;
		} else if (wally->scancode == KEY_ENTER) {
			wally->scancode = 0;
			if (wally->menu == 0) {
				if (wally->option == 0) {
					wally->menu = 1;
					wally->timeLimit = 10*60;
				} else
					wally->exit = 1;
			}
		}
	}
}

void drawWallyNIX(WallyNIX* wally) {
	if (wally->menu == 0) {
		fillDisplay(COLOUR_WHITE);
		if (wally->option == 0) {
			drawRectangle(100, 100, 500, 200, COLOUR_BLUE);
			drawRectangle(100, 300, 500, 400, COLOUR_BLACK);
		} else if (wally->option == 1) {
			drawRectangle(100, 100, 500, 200, COLOUR_BLACK);
			drawRectangle(100, 300, 500, 400, COLOUR_BLUE);
		}
	} else if (wally->menu == 1) {
		drawStage(wally->level1);

		int number = wally->timeLimit / 60;
		switch (number) {
		case 0:
			drawTimer(wally->char0);
			break;
		case 1:
			drawTimer(wally->char1);
			break;
		case 2:
			drawTimer(wally->char2);
			break;
		case 3:
			drawTimer(wally->char3);
			break;
		case 4:
			drawTimer(wally->char4);
			break;
		case 5:
			drawTimer(wally->char5);
			break;
		case 6:
			drawTimer(wally->char6);
			break;
		case 7:
			drawTimer(wally->char7);
			break;
		case 8:
			drawTimer(wally->char8);
			break;
		case 9:
			drawTimer(wally->char9);
			break;
		default:
			break;
		}
	}
}

void stopWallyNIX(WallyNIX* wally) {
	//Unsubscribe devices
	unsubscribeTimer();
	unsubscribeKeyboard();
	unsubscribeMouse();

	deleteTimer(wally->timer);
	free(wally);

	exitGraphics();
}

