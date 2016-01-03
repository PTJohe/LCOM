#include <minix/drivers.h>
#include "WallyNIX.h"

#include "Utilities.h"
#include "Bitmap.h"
#include "Game.h"
#include "Graphics.h"
#include "Keyboard.h"
#include "Mouse.h"
#include "Timer.h"

WallyNIX* startWallyNIX() {
	WallyNIX* wally = (WallyNIX*) malloc(sizeof(WallyNIX));

	wally->menu = MAIN_MENU;

	wally->exit = 0;
	wally->scancode = 0;

	// Subscribe devices
	wally->IRQ_SET_KBD = subscribeKeyboard();
	wally->IRQ_SET_TIMER = subscribeTimer();
	wally->IRQ_SET_MOUSE = subscribeMouse();

	wally->mainMenu = createMainMenu();
	wally->timerFPS = createTimer();

	return wally;
}

void updateWallyNIX(WallyNIX* wally) {
	updateInterruptions(wally);
	updateKeyboard(wally);
	updateStates(wally);
}

void updateInterruptions(WallyNIX* wally) {
	int ipc_status, r = 0;
	message msg;

	//INTERRUPTIONS HANDLER
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
				timerCount(wally->timerFPS);
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
}
void updateKeyboard(WallyNIX* wally) {

	//READ KEYBOARD
	switch (wally->scancode) {
	case KEY_ESC:
		wally->scancode = 0;
		if (wally->menu == MAIN_MENU) {
			getMouse()->draw = 0;
			wally->mainMenu->option = 4;
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
				wally->stageSelect->option = 9;
		} else if (wally->menu == HIGH_SCORES) {
			if (wally->highScores->newScore) {

			} else
				wally->highScores->option = 0;
		} else if (wally->menu == OPTIONS) {
			wally->options->option = 7;
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
			if (wally->stageSelect->currentStage == 0) {
				if (wally->stageSelect->option < 0)
					wally->stageSelect->option = 0;
				else if (wally->stageSelect->option == 0
						|| wally->stageSelect->option == 2)
					break;
				else if (wally->stageSelect->option == 9)
					wally->stageSelect->option = 7;
				else
					wally->stageSelect->option -= 3;
			} else if (wally->stageSelect->option - 1 >= 0)
				wally->stageSelect->option -= 1;
		} else if (wally->menu == HIGH_SCORES) {
			if (wally->highScores->newScore) {

			} else
				wally->highScores->option = 0;
		} else if (wally->menu == OPTIONS) {
			if (wally->options->option < 0)
				wally->options->option = 0;
			else if (wally->options->option < 4)
				break;
			else if (wally->options->option < 6)
				wally->options->option = 0;
			else
				wally->options->option = 4;
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
			if (wally->stageSelect->currentStage == 0) {
				if (wally->stageSelect->option < 0)
					wally->stageSelect->option = 0;
				else if (wally->stageSelect->option > 6)
					wally->stageSelect->option = 9;
				else
					wally->stageSelect->option += 3;
			} else if (wally->stageSelect->pause) {
				if (wally->stageSelect->option + 1 <= 1)
					wally->stageSelect->option += 1;
			}
		} else if (wally->menu == HIGH_SCORES) {
			if (wally->highScores->newScore) {

			} else
				wally->highScores->option = 0;
		} else if (wally->menu == OPTIONS) {
			if (wally->options->option < 0)
				wally->options->option = 0;
			else if (wally->options->option < 4)
				wally->options->option = 4;
			else if (wally->options->option < 6)
				wally->options->option = 6;
		}
		break;
	case KEY_A:
		wally->scancode = 0;
		if (wally->menu == STAGE_SELECT) {
			if (wally->stageSelect->currentStage == 0) {
				if (wally->stageSelect->option < 0)
					wally->stageSelect->option = 0;
				else if (wally->stageSelect->option == 9)
					wally->stageSelect->option = 6;
				else if (wally->stageSelect->option % 3 == 0)
					break;
				else
					wally->stageSelect->option--;
			}
		} else if (wally->menu == HIGH_SCORES) {
			if (wally->highScores->newScore) {

			} else
				wally->highScores->option = 0;
		} else if (wally->menu == OPTIONS) {
			if (wally->options->option <= 0)
				wally->options->option = 0;
			else if (wally->options->option < 4)
				wally->options->option--;
			else if (wally->options->option < 6)
				wally->options->option = 4;
			else
				wally->options->option = 6;
		}
		break;
	case KEY_D:
		wally->scancode = 0;
		if (wally->menu == STAGE_SELECT) {
			if (wally->stageSelect->currentStage == 0) {
				if (wally->stageSelect->option < 0)
					wally->stageSelect->option = 0;
				else if (wally->stageSelect->option == 9)
					wally->stageSelect->option = 8;
				else if ((wally->stageSelect->option - 2) % 3 == 0)
					break;
				else
					wally->stageSelect->option++;
			}
			break;
		} else if (wally->menu == HIGH_SCORES) {
			if (wally->highScores->newScore) {

			} else
				wally->highScores->option = 0;
		} else if (wally->menu == OPTIONS) {
			if (wally->options->option < 0)
				wally->options->option = 0;
			else if (wally->options->option < 3)
				wally->options->option++;
			else if (wally->options->option == 3)
				break;
			else if (wally->options->option < 6)
				wally->options->option = 5;
			else
				wally->options->option = 7;
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
			case 2:
				wally->menu = HIGH_SCORES;
				wally->highScores = createHighScores(0);
				break;
			case 3:
				deleteMainMenu(wally->mainMenu);
				wally->menu = OPTIONS;
				wally->options = createOptions();
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
						returnToStageSelect(wally->stageSelect);
					}
				}
			} else if (wally->stageSelect->option == 9) {
				wally->stageSelect->done = 1;
			} else if (wally->stageSelect->option >= 0) {
				pickStageSelect(wally->stageSelect, wally->stageSelect->option);
			}
		} else if (wally->menu == HIGH_SCORES) {
			if (wally->highScores->newScore) {

			} else
				wally->highScores->done = 1;
		} else if (wally->menu == OPTIONS) {
			if (wally->options->option < 0)
				break;
			else if (wally->options->option < 4)
				wally->options->selectedCursor = wally->options->option + 1;
			else if (wally->options->option == 4) {
				if (wally->options->selectedSensitivity > 0)
					wally->options->selectedSensitivity--;
			} else if (wally->options->option == 5) {
				if (wally->options->selectedSensitivity < 5)
					wally->options->selectedSensitivity++;
			} else if (wally->options->option == 6) {
				setMouseOptions(wally->options->selectedCursor,
						wally->options->selectedSensitivity);
				wally->options->done = 1;
			} else if (wally->options->option == 7) {
				wally->options->done = 1;
			}
		}
		break;
	default:
		break;
	}
}
void updateStates(WallyNIX* wally) {

	//UPDATE STATES
	switch (wally->menu) {
	case MAIN_MENU:
		updateMainMenu(wally->mainMenu);

		switch (wally->mainMenu->mouseSelection) {
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
		case 2:
			deleteMainMenu(wally->mainMenu);
			wally->menu = HIGH_SCORES;
			wally->highScores = createHighScores(0);
			break;
		case 3:
			deleteMainMenu(wally->mainMenu);
			wally->menu = OPTIONS;
			wally->options = createOptions();
			break;
		case 4:
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
			return;
		}
		break;
	case STAGE_SELECT:
		updateStageSelect(wally->stageSelect);

		if (wally->stageSelect->currentStage == 0) {
			if (wally->stageSelect->done
					|| wally->stageSelect->mouseSelection == 9) {
				deleteStageSelect(wally->stageSelect);
				wally->menu = MAIN_MENU;
				wally->mainMenu = createMainMenu();
				return;
			} else if (wally->stageSelect->mouseSelection != -1) {
				pickStageSelect(wally->stageSelect,
						wally->stageSelect->mouseSelection + 1);
			}
		}
		break;
	case HIGH_SCORES:
		updateHighScores(wally->highScores);

		if (wally->highScores->done || wally->highScores->mouseSelection == 0) {
			deleteHighScores(wally->highScores);
			wally->menu = MAIN_MENU;
			wally->mainMenu = createMainMenu();
			return;
		}
		break;
	case OPTIONS:
		updateOptions(wally->options);

		if (wally->options->done) {
			deleteOptions(wally->options);
			wally->menu = MAIN_MENU;
			wally->mainMenu = createMainMenu();
			return;
		} else if (wally->options->mouseSelection < 0) {
			break;
		} else if (wally->options->mouseSelection < 4)
			wally->options->selectedCursor = wally->options->mouseSelection + 1;
		else if (wally->options->mouseSelection == 4) {
			if (wally->options->selectedSensitivity > 0) {
				wally->options->selectedSensitivity--;
				resetMouseButton();
				wally->options->mouseSelection = -1;
			}
		} else if (wally->options->mouseSelection == 5) {
			if (wally->options->selectedSensitivity < 5) {
				wally->options->selectedSensitivity++;
				resetMouseButton();
				wally->options->mouseSelection = -1;
			}
		} else if (wally->options->mouseSelection == 6) {
			setMouseOptions(wally->options->selectedCursor,
					wally->options->selectedSensitivity);
			wally->options->done = 1;
		} else if (wally->options->mouseSelection == 7) {
			wally->options->done = 1;
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
		break;
	case HIGH_SCORES:
		drawHighScores(wally->highScores);
		break;
	case OPTIONS:
		drawOptions(wally->options);
		break;
	default:
		break;
	}
}

void stopWallyNIX(WallyNIX* wally) {
	deleteTimer(wally->timerFPS);
	deleteMouse();

	deleteMainMenu(wally->mainMenu);

	//Unsubscribe devices
	unsubscribeKeyboard();
	unsubscribeTimer();
	unsubscribeMouse();

	free(wally);
}

