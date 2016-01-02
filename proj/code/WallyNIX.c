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

	//INTERRUPTIONS HANDLERS
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
					if (wally->arcadeMode->timer->enabled == 1)
						timerCount(wally->arcadeMode->timer);
				} else if (wally->menu == STAGE_SELECT) {
					if (wally->stageSelect->timer->enabled == 1)
						timerCount(wally->stageSelect->timer);
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

	//UPDATE STATES
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
		case 2:
			deleteMainMenu(wally->mainMenu);
			wally->menu = STAGE_SELECT;
			wally->stageSelect = createStageSelect();
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
			wally->option = 0;
			return;
		}
		break;
	case STAGE_SELECT:
		updateStageSelect(wally->stageSelect);

		if (wally->stageSelect->currentStage == 0) {
			if (wally->stageSelect->done
					|| wally->stageSelect->mouseSelection == 5) {
				deleteStageSelect(wally->stageSelect);
				wally->menu = MAIN_MENU;
				wally->mainMenu = createMainMenu();
				wally->option = 0;
				return;
			} else if (wally->stageSelect->mouseSelection) {
				wally->stageSelect->currentStage =
						wally->stageSelect->mouseSelection;
				wally->stageSelect->mouseSelection = 0;
				resetTimer(wally->stageSelect->timer);
				startTimer(wally->stageSelect->timer);
			}
		}
		break;
	default:
		break;
	}

	//READ KEYBOARD
	switch (wally->scancode) {
	case KEY_ESC:
		wally->scancode = 0;
		if (wally->menu == MAIN_MENU) {
			wally->exit = 1;
		} else if (wally->menu == ARCADE_MODE) {
			if (wally->arcadeMode->pause) {
				wally->arcadeMode->pause = 0;
				resumeTimer(wally->arcadeMode->timer);
			} else {
				stopTimer(wally->arcadeMode->timer);
				wally->arcadeMode->pause = 1;
				wally->arcadeMode->option = -1;
			}
		} else if (wally->menu == STAGE_SELECT) {
			if (wally->stageSelect->currentStage) {
				if (wally->stageSelect->pause) {
					wally->stageSelect->pause = 0;
					resumeTimer(wally->stageSelect->timer);
				} else {
					stopTimer(wally->stageSelect->timer);
					wally->stageSelect->pause = 1;
					wally->stageSelect->option = -1;
				}
			} else
				wally->stageSelect->done;
		}
		break;
	case KEY_W:
		wally->scancode = 0;
		getMouse()->draw = 0;

		if (wally->menu == MAIN_MENU) {
			if (wally->mainMenu->option - 1 >= 0)
				wally->mainMenu->option -= 1;
		} else if (wally->menu == ARCADE_MODE) {
			if (wally->arcadeMode->option - 1 >= 0)
				wally->arcadeMode->option -= 1;
		} else if (wally->menu == STAGE_SELECT) {
			if (wally->stageSelect->option - 1 >= 0)
				wally->stageSelect->option -= 1;
		}
		break;
	case KEY_S:
		wally->scancode = 0;
		getMouse()->draw = 0;

		if (wally->menu == MAIN_MENU) {
			if (wally->mainMenu->option + 1 <= 4)
				wally->mainMenu->option += 1;
		} else if (wally->menu == ARCADE_MODE) {
			if (wally->arcadeMode->option + 1 <= 1)
				wally->arcadeMode->option += 1;
		} else if (wally->menu == STAGE_SELECT) {
			if (wally->stageSelect->currentStage) {
				if (wally->stageSelect->pause) {
					if (wally->stageSelect->option + 1 <= 1)
						wally->stageSelect->option += 1;
				}
			} else if (wally->stageSelect->option + 1 <= 4)
				wally->stageSelect->option += 1;
		}
		break;
	case KEY_ENTER:
		wally->scancode = 0;
		if (wally->menu == MAIN_MENU) {
			switch (wally->mainMenu->option) {
			case 0:
				deleteMainMenu(wally->mainMenu);
				wally->menu = ARCADE_MODE;
				wally->arcadeMode = createArcadeMode();
				startTimer(wally->arcadeMode->timer);
				break;
			case 1:
				deleteMainMenu(wally->mainMenu);
				wally->menu = STAGE_SELECT;
				wally->stageSelect = createStageSelect();
				break;
			case 4:
				wally->exit = 1;
				break;
			default:
				break;
			}
		} else if (wally->menu == ARCADE_MODE) {
			if (wally->arcadeMode->pause) {
				if (wally->arcadeMode->option == 0) {
					wally->arcadeMode->pause = 0;
					resumeTimer(wally->arcadeMode->timer);
				} else if (wally->arcadeMode->option == 1) {
					wally->arcadeMode->done = 1;
				}
			}
		} else if (wally->menu == STAGE_SELECT) {
			if (wally->stageSelect->currentStage) {
				if (wally->stageSelect->pause) {
					if (wally->stageSelect->option == 0) {
						wally->stageSelect->pause = 0;
						resumeTimer(wally->stageSelect->timer);
					} else if (wally->stageSelect->option == 1) {
						resetStage(
								wally->stageSelect->stages[wally->stageSelect->currentStage
										- 1]);
						stopTimer(wally->stageSelect->timer);
						wally->stageSelect->currentStage = 0;
					}
				}
			} else if (wally->stageSelect->option == 4) {
				wally->stageSelect->done = 1;
			} else if (wally->stageSelect->option >= 0) {
				wally->stageSelect->currentStage = wally->stageSelect->option
						+ 1;
				startTimer(wally->stageSelect->timer);
			}
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
	case STAGE_SELECT:
		drawStageSelect(wally->stageSelect);
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

