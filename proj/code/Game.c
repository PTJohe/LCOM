#include "Game.h"

#include "Utilities.h"
#include "Keyboard.h"
#include "Mouse.h"
#include "RTC.h"

#include <minix/syslib.h>
#include <minix/drivers.h>
#include <minix/com.h>
#include <stdio.h>
#include <stdlib.h>

//==========================================================================================
Button* createButton(int xi, int yi, int xf, int yf, char* text) {
	Button* button = (Button*) malloc(sizeof(Button));

	button->topLeft = createPosition(xi, yi);
	button->bottomRight = createPosition(xf, yf);

	if (strcmp(text, "") == 0)
		button->text = NULL;
	else
		button->text = loadBitmap(getImagePath(text));

	button->onHover = 0;
	button->onClick = 0;

	return button;
}
int mouseInButton(Button* button) {
	Mouse* mouse = getMouse();

	if ((button->topLeft->x < mouse->position->x
			&& mouse->position->x < button->bottomRight->x)
			&& (button->topLeft->y < mouse->position->y
					&& mouse->position->y < button->bottomRight->y))
		return 1; //Mouse inside button
	else
		return 0; //Mouse outside button
}
void updateButton(Button* button) {
	if (getMouse()->draw)
		if (mouseInButton(button) == 1) {
			button->onHover = 1;

			if (getMouse()->leftButtonReleased == 1) {
				button->onClick = 1;
			} else
				button->onClick = 0;
		} else {
			button->onHover = 0;
			button->onClick = 0;
		}
	else {
		button->onHover = 0;
		button->onClick = 0;
	}
}
void drawButton(Button* button, int highlight) {
	unsigned long colour = COLOUR_BLACK;

	if (getMouse()->draw) {
		if (highlight == 2)
			colour = COLOUR_WHITE;
		if (button->onHover == 1)
			colour = COLOUR_ORANGE;
	} else if (highlight == 1)
		colour = COLOUR_ORANGE;
	else if (highlight == 2)
		colour = COLOUR_WHITE;

	drawRectangle(button->topLeft->x, button->topLeft->y,
			button->bottomRight->x, button->bottomRight->y, colour);

	int x = (int) round((button->topLeft->x + button->bottomRight->x) / 2.0);
	int y = button->topLeft->y + 5;
	drawBitmap(button->text, x, y, ALIGN_CENTER);
}
void deleteButton(Button* button) {
	deletePosition(button->topLeft);
	deletePosition(button->bottomRight);
	deleteBitmap(button->text);
	free(button);
}

//==========================================================================================
MainMenu* createMainMenu() {
	MainMenu* mainMenu = (MainMenu*) malloc(sizeof(MainMenu));

	mainMenu->background = loadBitmap((char*) getImagePath("mainMenu"));
	mainMenu->mouseSelection = -1;
	mainMenu->option = -1;
	mainMenu->done = 0;

	Button* arcadeMode = createButton(700, 300, 990, 370, "arcadeMode");
	Button* stageSelect = createButton(700, 380, 990, 450, "stageSelect");
	Button* highScores = createButton(700, 460, 990, 530, "highScores");
	Button* options = createButton(700, 540, 990, 610, "options");
	Button* exit = createButton(700, 620, 990, 690, "exitGame");
	Button* credits = createButton(515, 922, 765, 987, "creditsButton");

	mainMenu->buttons[0] = arcadeMode;
	mainMenu->buttons[1] = stageSelect;
	mainMenu->buttons[2] = highScores;
	mainMenu->buttons[3] = options;
	mainMenu->buttons[4] = exit;
	mainMenu->buttons[5] = credits;

	return mainMenu;
}
void updateMainMenu(MainMenu* mainMenu) {
	if (getMouse()->draw)
		mainMenu->option = -1;

	int i;
	for (i = 0; i < 6; i++) {
		updateButton(mainMenu->buttons[i]);
		if (mainMenu->buttons[i]->onClick) {
			mainMenu->mouseSelection = i;
			resetMouseButton();
		}
	}
}
void drawMainMenu(MainMenu* mainMenu) {
	drawBitmap(mainMenu->background, 0, 0, ALIGN_LEFT);
	int i;
	if (getMouse()->draw) {
		for (i = 0; i < 6; i++) {
			drawButton(mainMenu->buttons[i], 0);
		}
	} else {
		for (i = 0; i < 6; i++) {
			if (mainMenu->option == i)
				drawButton(mainMenu->buttons[i], 1);
			else
				drawButton(mainMenu->buttons[i], 0);
		}
	}
}
void deleteMainMenu(MainMenu* mainMenu) {
	deleteBitmap(mainMenu->background);

	int i;
	for (i = 0; i < 6; i++) {
		deleteButton(mainMenu->buttons[i]);
	}
	free(mainMenu);
}

//==========================================================================================
ArcadeMode* createArcadeMode() {
	ArcadeMode* arcadeMode = (ArcadeMode*) malloc(sizeof(ArcadeMode));

	arcadeMode->score = 0;
	arcadeMode->currentStage = 1;
	arcadeMode->foundWally = 0;

	arcadeMode->pause = 0;
	arcadeMode->pauseContinue = createButton(515, 500, 805, 570,
			"pauseContinue");
	arcadeMode->pauseQuit = createButton(515, 580, 805, 650, "pauseQuit");

	arcadeMode->option = -1;
	arcadeMode->gameOver = 0;
	arcadeMode->done = 0;

	arcadeMode->findWally = loadBitmap(getImagePath("findWally"));
	arcadeMode->wally = loadBitmap(getImagePath("wally"));
	arcadeMode->checkmark = loadBitmap(getImagePath("checkmark"));

	arcadeMode->timer = createTimer();

	FILE *file = fopen(PATH_STAGE "coordArcade.txt", "r");
	if (file == NULL) {
		LOG("ERROR", "Couldn't open coordinates file.");
	}

	int i = 0;
	char str[100];
	while (fgets(str, 100, file)) {
		int stageNumber = atoi(str);
		fgets(str, 100, file);
		char* sub = strtok(str, " ");
		int x = atoi(sub);
		sub = strtok(NULL, " ");
		int y = atoi(sub);
		Position* position = createPosition(x, y + 213);

		arcadeMode->stages[i] = createStageArcadeMode(stageNumber, position);
		i++;
	}
	fclose(file);

	return arcadeMode;
}
void updateArcadeMode(ArcadeMode* arcadeMode) {
	if (getMouse()->draw)
		arcadeMode->option = -1;

	if (arcadeMode->foundWally) {
		int waiting = arcadeMode->timer->counter - arcadeMode->foundWally;
		if (waiting >= 1 * TIMER_DEFAULT_FREQ) {
			if (arcadeMode->currentStage == NUM_STAGES) {
				arcadeMode->done = 1;
				return;
			}
			arcadeMode->currentStage++;
			arcadeMode->foundWally = 0;
			resetTimer(arcadeMode->timer);
			return;
		}
		return;
	}

	if (arcadeMode->pause) {
		updateButton(arcadeMode->pauseContinue);
		updateButton(arcadeMode->pauseQuit);

		if (arcadeMode->pauseContinue->onClick) {
			arcadeMode->pause = 0;
			resumeTimer(arcadeMode->timer);
		} else if (arcadeMode->pauseQuit->onClick) {
			arcadeMode->done = 1;
			resetMouseButton();
			return;
		}
		return;
	}

	Stage* stage = arcadeMode->stages[arcadeMode->currentStage - 1];

	updateStage(stage);

	if (stage->pause->onClick) {
		stopTimer(arcadeMode->timer);
		arcadeMode->pause = 1;
	}

	if (stage->foundWally) {
		drawCircleBorder(stage->wally->x, stage->wally->y, 25, COLOUR_ORANGE);
		arcadeMode->foundWally = arcadeMode->timer->counter;

		int timeLeft = (60 - arcadeMode->timer->counter / TIMER_DEFAULT_FREQ);
		arcadeMode->score += 10 * timeLeft;

		LOG_VAR("Time Left", timeLeft);
		LOG_VAR("Score", arcadeMode->score);
	}

	if (arcadeMode->timer->counter >= 60 * TIMER_DEFAULT_FREQ) {
		arcadeMode->done = 1;
		arcadeMode->gameOver = 1;
	}
}
void drawArcadeMode(ArcadeMode* arcadeMode) {
	if (arcadeMode->foundWally) {
		drawBitmap(arcadeMode->checkmark, 350, 25, ALIGN_LEFT);
		return;
	}

	if (arcadeMode->pause) {
		drawRectangle(505, 490, 815, 660, COLOUR_GREY);
		if (getMouse()->draw) {
			drawButton(arcadeMode->pauseContinue, 0);
			drawButton(arcadeMode->pauseQuit, 0);
		} else {
			switch (arcadeMode->option) {
			case -1:
				drawButton(arcadeMode->pauseContinue, 0);
				drawButton(arcadeMode->pauseQuit, 0);
				break;
			case 0:
				drawButton(arcadeMode->pauseContinue, 1);
				drawButton(arcadeMode->pauseQuit, 0);
				break;
			case 1:
				drawButton(arcadeMode->pauseContinue, 0);
				drawButton(arcadeMode->pauseQuit, 1);
				break;
			default:
				break;
			}
		}
		return;
	}

	fillDisplay(COLOUR_WHITE);
	drawTimeLeft(arcadeMode->timer, 60);

	drawBitmap(arcadeMode->findWally, 0, 30, ALIGN_LEFT);
	drawBitmap(arcadeMode->wally, 280, 10, ALIGN_LEFT);
	drawStage(arcadeMode->stages[arcadeMode->currentStage - 1]);
}
void deleteArcadeMode(ArcadeMode* arcadeMode) {
	int i;
	for (i = 0; i < NUM_STAGES; i++) {
		deleteStage(arcadeMode->stages[i]);
	}

	deleteButton(arcadeMode->pauseContinue);
	deleteButton(arcadeMode->pauseQuit);
	deleteBitmap(arcadeMode->findWally);
	deleteBitmap(arcadeMode->wally);
	deleteBitmap(arcadeMode->checkmark);
	deleteTimer(arcadeMode->timer);

	free(arcadeMode);
}

//==========================================================================================
StageSelect* createStageSelect() {
	StageSelect* stageSelect = (StageSelect*) malloc(sizeof(StageSelect));

	stageSelect->currentStage = 0;
	stageSelect->foundAll = 0;

	stageSelect->background = loadBitmap(getImagePath("stageSelectMenu"));

	int i;
	int posX = 220, posY = 240;
	for (i = 1; i < (NUM_STAGES + 1); i++) {
		char str[128];
		sprintf(str, "stage0%d", i);
		stageSelect->buttons[i - 1] = createButton(posX, posY, posX + 260,
				posY + 160, str);
		posX += 280;
		if (i % 3 == 0) {
			posX = 220;
			posY += 180;
		}
	}

	stageSelect->buttons[9] = createButton(535, 850, 745, 910,
			"mainMenuButton");

	stageSelect->pause = 0;
	stageSelect->pauseContinue = createButton(515, 500, 805, 570,
			"pauseContinue");
	stageSelect->pauseQuit = createButton(515, 580, 805, 650, "pauseQuit");

	stageSelect->mouseSelection = -1;
	stageSelect->option = -1;
	stageSelect->done = 0;

	stageSelect->checkmark = loadBitmap(getImagePath("checkmark"));

	stageSelect->findWally = loadBitmap(getImagePath("findWally"));
	stageSelect->wally = loadBitmap(getImagePath("wally"));
	stageSelect->findWenda = loadBitmap(getImagePath("findWenda"));
	stageSelect->wenda = loadBitmap(getImagePath("wenda"));
	stageSelect->findWhitebeard = loadBitmap(getImagePath("findWhitebeard"));
	stageSelect->whitebeard = loadBitmap(getImagePath("whitebeard"));
	stageSelect->findOdlaw = loadBitmap(getImagePath("findOdlaw"));
	stageSelect->odlaw = loadBitmap(getImagePath("odlaw"));

	stageSelect->timer = createTimer();

	FILE *file = fopen(PATH_STAGE "coordStageSelect.txt", "r");
	if (file == NULL) {
		LOG("ERROR", "Couldn't open coordinates file.");
	}

	i = 0;
	char str[100];
	while (fgets(str, 100, file)) {
		int stageNumber = atoi(str);
		fgets(str, 100, file);
		char* sub = strtok(str, " ");
		int x = atoi(sub);
		sub = strtok(NULL, " ");
		int y = atoi(sub);
		Position* positionWally = createPosition(x, y + 213);

		fgets(str, 100, file);
		sub = strtok(str, " ");
		x = atoi(sub);
		sub = strtok(NULL, " ");
		y = atoi(sub);
		Position* positionWenda = createPosition(x, y + 213);

		fgets(str, 100, file);
		sub = strtok(str, " ");
		x = atoi(sub);
		sub = strtok(NULL, " ");
		y = atoi(sub);
		Position* positionWhitebeard = createPosition(x, y + 213);

		fgets(str, 100, file);
		sub = strtok(str, " ");
		x = atoi(sub);
		sub = strtok(NULL, " ");
		y = atoi(sub);
		Position* positionOdlaw = createPosition(x, y + 213);

		stageSelect->stages[i] = createStage(stageNumber, positionWally,
				positionWenda, positionWhitebeard, positionOdlaw);
		i++;
	}
	fclose(file);

	return stageSelect;
}
void updateStageSelect(StageSelect* stageSelect) {
	if (getMouse()->draw)
		stageSelect->option = -1;

	if (stageSelect->currentStage) {
		if (stageSelect->foundAll) {
			int waiting = stageSelect->timer->counter - stageSelect->foundAll;
			if (waiting >= 1 * TIMER_DEFAULT_FREQ) {
				returnToStageSelect(stageSelect);
				return;
			}
			return;
		}

		if (stageSelect->pause) {
			updateButton(stageSelect->pauseContinue);
			updateButton(stageSelect->pauseQuit);

			if (stageSelect->pauseContinue->onClick) {
				stageSelect->pause = 0;
				resumeTimer(stageSelect->timer);
			} else if (stageSelect->pauseQuit->onClick) {
				returnToStageSelect(stageSelect);
			}
			return;
		}

		Stage* stage = stageSelect->stages[stageSelect->currentStage - 1];
		updateStage(stage);

		if (stage->pause->onClick) {
			stopTimer(stageSelect->timer);
			stageSelect->pause = 1;
		}

		if (foundAll(stage))
			stageSelect->foundAll = stageSelect->timer->counter;

		if (stageSelect->timer->counter >= 120 * TIMER_DEFAULT_FREQ) {
			returnToStageSelect(stageSelect);
		}
	} else {
		if (getMouse()->draw)
			stageSelect->option = -1;

		int i;
		for (i = 0; i < (NUM_STAGES + 1); i++) {
			updateButton(stageSelect->buttons[i]);
			if (stageSelect->buttons[i]->onClick) {
				stageSelect->mouseSelection = i;
				resetMouseButton();
			}
		}
	}
}
void pickStageSelect(StageSelect* stageSelect, int option) {
	stageSelect->foundAll = 0;
	stageSelect->pause = 0;
	stageSelect->mouseSelection = -1;
	stageSelect->option = -1;
	stageSelect->currentStage = option;
	resetStage(stageSelect->stages[stageSelect->currentStage - 1]);
	startTimer(stageSelect->timer);
	resetMouseButton();
}
void returnToStageSelect(StageSelect* stageSelect) {
	stageSelect->foundAll = 0;
	stageSelect->pause = 0;
	stageSelect->mouseSelection = -1;
	stageSelect->option = -1;
	resetStage(stageSelect->stages[stageSelect->currentStage - 1]);
	stageSelect->currentStage = 0;
	stopTimer(stageSelect->timer);
	resetMouseButton();
}
void drawStageSelect(StageSelect* stageSelect) {
	if (stageSelect->currentStage) {
		if (stageSelect->timer->counter < 5 && !stageSelect->foundAll)
			fillDisplay(COLOUR_WHITE);
		if (stageSelect->stages[stageSelect->currentStage - 1]->foundWally)
			drawBitmap(stageSelect->checkmark, 400, 25, ALIGN_LEFT);
		if (stageSelect->stages[stageSelect->currentStage - 1]->foundWenda)
			drawBitmap(stageSelect->checkmark, 400, 95, ALIGN_LEFT);
		if (stageSelect->stages[stageSelect->currentStage - 1]->foundWhitebeard)
			drawBitmap(stageSelect->checkmark, 830, 25, ALIGN_LEFT);
		if (stageSelect->stages[stageSelect->currentStage - 1]->foundOdlaw)
			drawBitmap(stageSelect->checkmark, 830, 95, ALIGN_LEFT);

		if (stageSelect->pause) {
			drawRectangle(505, 490, 815, 660, COLOUR_GREY);
			if (getMouse()->draw) {
				drawButton(stageSelect->pauseContinue, 0);
				drawButton(stageSelect->pauseQuit, 0);
			} else {
				switch (stageSelect->option) {
				case -1:
					drawButton(stageSelect->pauseContinue, 0);
					drawButton(stageSelect->pauseQuit, 0);
					break;
				case 0:
					drawButton(stageSelect->pauseContinue, 1);
					drawButton(stageSelect->pauseQuit, 0);
					break;
				case 1:
					drawButton(stageSelect->pauseContinue, 0);
					drawButton(stageSelect->pauseQuit, 1);
					break;
				default:
					break;
				}
			}
			return;
		}

		if (!stageSelect->foundAll)
			drawTimeLeft(stageSelect->timer, 120);

		drawBitmap(stageSelect->findWally, 0, 30, ALIGN_LEFT);
		drawBitmap(stageSelect->wally, 380, 10, ALIGN_RIGHT);

		drawBitmap(stageSelect->findWenda, 0, 100, ALIGN_LEFT);
		drawBitmap(stageSelect->wenda, 380, 110, ALIGN_RIGHT);

		drawBitmap(stageSelect->findWhitebeard, 990, 30, ALIGN_LEFT);
		drawBitmap(stageSelect->whitebeard, 900, 10, ALIGN_LEFT);

		drawBitmap(stageSelect->findOdlaw, 990, 100, ALIGN_LEFT);
		drawBitmap(stageSelect->odlaw, 900, 110, ALIGN_LEFT);

		drawStage(stageSelect->stages[stageSelect->currentStage - 1]);
	} else {
		drawBitmap(stageSelect->background, 0, 0, ALIGN_LEFT);
		int i;
		if (getMouse()->draw) {
			for (i = 0; i < (NUM_STAGES + 1); i++) {
				drawButton(stageSelect->buttons[i], 0);
			}
		} else {
			for (i = 0; i < (NUM_STAGES + 1); i++) {
				if (stageSelect->option == i)
					drawButton(stageSelect->buttons[i], 1);
				else
					drawButton(stageSelect->buttons[i], 0);
			}
		}
	}
}
void deleteStageSelect(StageSelect* stageSelect) {
	int i;
	for (i = 0; i < NUM_STAGES; i++) {
		deleteStage(stageSelect->stages[i]);
		deleteButton(stageSelect->buttons[i]);
	}
	deleteButton(stageSelect->buttons[9]);

	deleteButton(stageSelect->pauseContinue);
	deleteButton(stageSelect->pauseQuit);
	deleteBitmap(stageSelect->checkmark);

	deleteBitmap(stageSelect->findWally);
	deleteBitmap(stageSelect->wally);
	deleteBitmap(stageSelect->findWenda);
	deleteBitmap(stageSelect->wenda);
	deleteBitmap(stageSelect->findWhitebeard);
	deleteBitmap(stageSelect->whitebeard);
	deleteBitmap(stageSelect->findOdlaw);
	deleteBitmap(stageSelect->odlaw);

	deleteTimer(stageSelect->timer);

	free(stageSelect);
}

//==========================================================================================
Stage* createStageArcadeMode(int stageNumber, Position* wally) {
	Stage* stage = (Stage*) malloc(sizeof(Stage));

	stage->arcadeMode = 1;

	stage->foundWally = 0;
	stage->wally = wally;

	stage->pause = createButton(1050, 30, 1240, 100, "pause");
	stage->image = loadBitmap(getStagePath(stageNumber));

	return stage;
}
Stage* createStage(int stageNumber, Position* wally, Position* wenda,
		Position* whitebeard, Position* odlaw) {
	Stage* stage = (Stage*) malloc(sizeof(Stage));

	stage->arcadeMode = 0;

	stage->foundWally = 0;
	stage->wally = wally;
	stage->foundWenda = 0;
	stage->wenda = wenda;
	stage->foundWhitebeard = 0;
	stage->whitebeard = whitebeard;
	stage->foundOdlaw = 0;
	stage->odlaw = odlaw;

	stage->pause = createButton(550, 120, 740, 190, "pause");
	stage->image = loadBitmap(getStagePath(stageNumber));

	return stage;
}
int foundAll(Stage* stage) {
	if (stage->foundWally && stage->foundWenda && stage->foundWhitebeard
			&& stage->foundOdlaw)
		return 1;
	else
		return 0;
}
void updateStage(Stage* stage) {
	updateButton(stage->pause);

	if (stage->pause->onClick) {
		stage->pause->onHover = 0;
		drawButton(stage->pause, 0);
		return;
	}

	Mouse* mouse = getMouse();
	if (mouse->leftButtonReleased == 1) {
		if (calcDistance(mouse->position, stage->wally) < 25)
			stage->foundWally = 1;
		if (!stage->arcadeMode) {
			if (calcDistance(mouse->position, stage->wenda) < 25)
				stage->foundWenda = 1;
			if (calcDistance(mouse->position, stage->whitebeard) < 25)
				stage->foundWhitebeard = 1;
			if (calcDistance(mouse->position, stage->odlaw) < 25)
				stage->foundOdlaw = 1;
		}
	}
}
void drawStage(Stage* stage) {
	drawButton(stage->pause, 0);
	drawBitmap(stage->image, 0, 213, ALIGN_LEFT);

	if (!stage->arcadeMode) {
		if (stage->foundWally) {
			drawCircleBorder(stage->wally->x, stage->wally->y, 25,
			COLOUR_ORANGE);
		}
		if (stage->foundWenda) {
			drawCircleBorder(stage->wenda->x, stage->wenda->y, 25,
			COLOUR_ORANGE);
		}
		if (stage->foundWhitebeard) {
			drawCircleBorder(stage->whitebeard->x, stage->whitebeard->y, 25,
			COLOUR_ORANGE);
		}
		if (stage->foundOdlaw) {
			drawCircleBorder(stage->odlaw->x, stage->odlaw->y, 25,
			COLOUR_ORANGE);
		}
	}
}
void resetStage(Stage* stage) {
	stage->foundWally = 0;
	stage->foundWenda = 0;
	stage->foundWhitebeard = 0;
	stage->foundOdlaw = 0;
}
void deleteStage(Stage* stage) {
	deleteButton(stage->pause);
	deleteBitmap(stage->image);

	deletePosition(stage->wally);
	if (!stage->arcadeMode) {
		deletePosition(stage->wenda);
		deletePosition(stage->whitebeard);
		deletePosition(stage->odlaw);
	}

	free(stage);
}

//==========================================================================================
HighScores* createHighScores(int score) {
	HighScores* highScores = (HighScores*) malloc(sizeof(HighScores));

	highScores->draw = 1;
	highScores->mouseSelection = -1;
	highScores->option = -1;
	highScores->done = 0;

	FILE *file = fopen(PATH_STAGE "highScores.txt", "r");
	if (file == NULL) {
		LOG("ERROR", "Couldn't open highScores file.");
		return highScores;
	}
	int i = 0;
	char str[100];
	while (fgets(str, 100, file)) {
		fgets(str, 100, file);
		char* name = (char*) malloc(100);
		strcpy(name, str);
		strtok(name, "\n");

		fgets(str, 100, file);
		int score = atoi(str);

		fgets(str, 100, file);
		char* date = (char*) malloc(100);
		strcpy(date, str);
		strtok(date, "\n");

		highScores->scores[i] = createScore(name, score, date);
		LOG("Name", name);
		LOG_VAR("Score", score);
		LOG("Date", date);
		i++;
	}
	fclose(file);
	sortHighScores(highScores);

	if (score > highScores->scores[9]->score) {
		highScores->inputName = 1;
		highScores->points = score;
		highScores->input[0] = '\0';
		highScores->newScore = loadBitmap(getImagePath("writeName"));
		highScores->ok = createButton(595, 550, 685, 630, "ok");
	} else {
		highScores->inputName = 0;
		highScores->addScore = 0;
	}
	highScores->background = loadBitmap(getImagePath("highscoresMenu"));
	highScores->button = createButton(535, 850, 745, 910, "mainMenuButton");

	return highScores;
}
void sortHighScores(HighScores* highScores) {
	int i, j;

	Score* temp = NULL;

	for (i = 0; i < 10; i++) {
		for (j = i + 1; j < 10; j++) {
			if (highScores->scores[i]->score < highScores->scores[j]->score) {
				temp = highScores->scores[i];
				highScores->scores[i] = highScores->scores[j];
				highScores->scores[j] = temp;
			}
		}
	}
}
void insertHighScore(HighScores* highScores, Score* score) {
	sortHighScores(highScores);
	int i;
	for (i = 9; highScores->scores[i]->score < score->score; i--) {
		if (i == 9) {
			highScores->scores[i] = score;
			continue;
		}
		highScores->scores[i + 1] = highScores->scores[i];
		highScores->scores[i] = score;

		if (i == 0)
			break;
	}
}
void updateHighScores(HighScores* highScores) {
	if (highScores->inputName) {
		updateButton(highScores->ok);
		if (highScores->ok->onClick) {
			if (strlen(highScores->input) == 0) {
				highScores->ok->onClick = 0;
				highScores->mouseSelection = -1;
				resetMouseButton();
				return;
			}
			highScores->mouseSelection = 0;
			resetMouseButton();
		}
		return;
	} else if (highScores->addScore) {
		char* date = getDateRTC();
		char* name = malloc(1 + strlen(highScores->input));
		strcpy(name, highScores->input);

		Score* score = createScore(name, highScores->points, date);
		insertHighScore(highScores, score);

		highScores->draw = 1;
		highScores->addScore = 0;
		highScores->input[0] = '\0';
		highScores->points = 0;
		deleteBitmap(highScores->newScore);
		deleteButton(highScores->ok);
		return;
	}

	if (getMouse()->draw)
		highScores->option = -1;

	updateButton(highScores->button);
	if (highScores->button->onClick) {
		highScores->mouseSelection = 0;
		resetMouseButton();
	}
}
void inputName(HighScores* highScores, unsigned long scancode) {
	if (scancode == KEY_ENTER) {
		if (strlen(highScores->input) == 0)
			return;
		highScores->inputName = 0;
		highScores->addScore = 1;
		LOG("Name", highScores->input);
	} else if (scancode == KEY_BACKSPACE) {
		int len = strlen(highScores->input);
		if (len > 0)
			highScores->input[len - 1] = '\0';
	} else if (strlen(highScores->input) < 11) {
		switch (scancode) {
		case KEY_SPACEBAR:
			strcat(highScores->input, " ");
			break;
		case KEY_Q:
			strcat(highScores->input, "Q");
			break;
		case KEY_W:
			strcat(highScores->input, "W");
			break;
		case KEY_E:
			strcat(highScores->input, "E");
			break;
		case KEY_R:
			strcat(highScores->input, "R");
			break;
		case KEY_T:
			strcat(highScores->input, "T");
			break;
		case KEY_Y:
			strcat(highScores->input, "Y");
			break;
		case KEY_U:
			strcat(highScores->input, "U");
			break;
		case KEY_I:
			strcat(highScores->input, "I");
			break;
		case KEY_O:
			strcat(highScores->input, "O");
			break;
		case KEY_P:
			strcat(highScores->input, "P");
			break;
		case KEY_A:
			strcat(highScores->input, "A");
			break;
		case KEY_S:
			strcat(highScores->input, "S");
			break;
		case KEY_D:
			strcat(highScores->input, "D");
			break;
		case KEY_F:
			strcat(highScores->input, "F");
			break;
		case KEY_G:
			strcat(highScores->input, "G");
			break;
		case KEY_H:
			strcat(highScores->input, "H");
			break;
		case KEY_J:
			strcat(highScores->input, "J");
			break;
		case KEY_K:
			strcat(highScores->input, "K");
			break;
		case KEY_L:
			strcat(highScores->input, "L");
			break;
		case KEY_Z:
			strcat(highScores->input, "Z");
			break;
		case KEY_X:
			strcat(highScores->input, "X");
			break;
		case KEY_C:
			strcat(highScores->input, "C");
			break;
		case KEY_V:
			strcat(highScores->input, "V");
			break;
		case KEY_B:
			strcat(highScores->input, "B");
			break;
		case KEY_N:
			strcat(highScores->input, "N");
			break;
		case KEY_M:
			strcat(highScores->input, "M");
			break;
		default:
			break;
		}
	}
}
void drawName(char* name, int rank) {
	int numChars = strlen(name); //max = 10
	int xMin = 200, xMax = 470;
	int xInc = 25;
	int x = 250;
	int y = 350 + ((rank - 1) * 50);

	char str[10];
	sprintf(str, "%s", name);

	int i;
	for (i = 0; i < strlen(str); i++) {
		Bitmap * bmp = loadBitmap(getFontPath(str[i]));
		drawBitmapAlpha(bmp, x, y, COLOUR_WHITE, 0);
		deleteBitmap(bmp);
		x += xInc;
	}
}
void drawScore(int points, int rank) {
	char score[15];
	sprintf(score, "%d", points);

	int numChars = strlen(score); //max = 8
	int xMin = 520, xMax = 700;
	int xInc = 25;
	int x = 575;
	int y = 350 + ((rank - 1) * 50);

	int i;
	for (i = 0; i < strlen(score); i++) {
		Bitmap * bmp = loadBitmap(getFontPath(score[i]));
		drawBitmapAlpha(bmp, x, y, COLOUR_WHITE, 0);
		deleteBitmap(bmp);
		x += xInc;
	}
}
void drawDate(char* date, int rank) {
	int numChars = strlen(date);
	int xMin = 800, xMax = 1200;
	int xInc1 = 22;
	int xInc2 = 20;
	int x = 800;
	int y = 350 + ((rank - 1) * 50);

	char str[20];
	sprintf(str, "%s", date);

	getDateRTC();

	int i;
	for (i = 0; i < strlen(str); i++) {
		Bitmap* bmp = NULL;
		if (str[i] == ' ') {
			x += xInc1 * 2;
			continue;
		} else if (str[i] == '/') {
			bmp = loadBitmap(getFontPath('-'));
			x -= 2;
		} else if (str[i] == ':') {
			bmp = loadBitmap(getFontPath('_'));
			x += 5;
			y += 5;
			drawBitmapAlpha(bmp, x, y, COLOUR_WHITE, 0);
			deleteBitmap(bmp);

			x += 15;
			y = 350 + ((rank - 1) * 50);
			continue;
		} else
			bmp = loadBitmap(getFontPath(str[i]));

		drawBitmapAlpha(bmp, x, y, COLOUR_WHITE, 0);
		deleteBitmap(bmp);
		if (i < 7)
			x += xInc1;
		else
			x += xInc2;
	}
}
void drawInput(HighScores* highScores) {
	int x = 530, y = 505;
	int xInc = 20;

	int i;
	for (i = 0; i < strlen(highScores->input); i++) {
		Bitmap * bmp = loadBitmap(getFontPath(highScores->input[i]));
		drawBitmapAlpha(bmp, x, y, COLOUR_WHITE, 0);
		deleteBitmap(bmp);
		x += xInc;
	}
}
void drawHighScores(HighScores* highScores) {
	if (highScores->draw) {
		highScores->draw = 0;
		drawBitmap(highScores->background, 0, 0, ALIGN_LEFT);

		int i;
		for (i = 0; i < 10; i++) {
			drawName(highScores->scores[i]->name, i);
			drawScore(highScores->scores[i]->score, i);
			drawDate(highScores->scores[i]->date, i);
		}
	}
	if (highScores->inputName) {
		drawBitmap(highScores->newScore, 375, 240, ALIGN_LEFT);
		drawInput(highScores);
		drawButton(highScores->ok, 0);
		return;
	}
	if (getMouse()->draw) {
		drawButton(highScores->button, 0);
	} else {
		if (highScores->option == 0)
			drawButton(highScores->button, 1);
		else
			drawButton(highScores->button, 0);
	}
}
void deleteHighScores(HighScores* highScores) {
	deleteBitmap(highScores->background);
	deleteButton(highScores->button);

	FILE *file = fopen(PATH_STAGE "highScores.txt", "w+");
	if (file == NULL) {
		LOG("ERROR", "Couldn't open highScores file.");
		free(highScores);
		return;
	}
	int i;
	for (i = 0; i < 10; i++) {
		fprintf(file, "%d\n", (i + 1));
		fprintf(file, "%s\n", highScores->scores[i]->name);
		fprintf(file, "%d\n", highScores->scores[i]->score);
		fprintf(file, "%s\n", highScores->scores[i]->date);

		deleteScore(highScores->scores[i]);
	}
	fclose(file);
	free(highScores);
}

//==========================================================================================
Score* createScore(char* name, int points, char* date) {
	Score* score = (Score*) malloc(sizeof(Score));

	score->name = name;
	score->score = points;
	score->date = date;

	return score;
}
void deleteScore(Score* score) {
	free(score->name);
	free(score->date);
	free(score);
}

//==========================================================================================
Options* createOptions() {
	Options* options = (Options*) malloc(sizeof(Options));

	options->mouseSelection = -1;
	options->option = -1;
	options->done = 0;

	options->selectedCursor = getMouse()->cursor;

	double sensitivity = getMouse()->sensitivity;
	if (sensitivity == 1.0)
		options->selectedSensitivity = 0;
	else if (sensitivity == 1.2)
		options->selectedSensitivity = 1;
	else if (sensitivity == 1.4)
		options->selectedSensitivity = 2;
	else if (sensitivity == 1.6)
		options->selectedSensitivity = 3;
	else if (sensitivity == 1.8)
		options->selectedSensitivity = 4;
	else if (sensitivity == 2.0)
		options->selectedSensitivity = 5;

	options->background = loadBitmap(getImagePath("optionsMenu"));

	int i;
	int posX = 775, posY = 375;
	for (i = 0; i < 4; i++) {
		char str[128];
		sprintf(str, "mouse%d", (i + 1));
		options->buttons[i] = createButton(posX, posY, posX + 24, posY + 36,
				str);
		posX += 50;
	}

	options->buttons[4] = createButton(650, 545, 710, 605, "buttonLess");
	options->buttons[5] = createButton(1060, 545, 1120, 605, "buttonMore");
	options->buttons[6] = createButton(440, 850, 630, 920, "apply");
	options->buttons[7] = createButton(650, 850, 840, 920, "cancel");

	return options;
}
void updateOptions(Options* options) {
	if (getMouse()->draw)
		options->option = -1;

	int i;
	for (i = 0; i < 8; i++) {
		updateButton(options->buttons[i]);
		if (options->buttons[i]->onClick) {
			options->mouseSelection = i;
			resetMouseButton();
		}
	}
}
void drawOptions(Options* options) {
	drawBitmap(options->background, 0, 0, ALIGN_LEFT);
	int i;
	if (getMouse()->draw) {
		for (i = 0; i < 4; i++) {
			if (options->selectedCursor == i + 1) {
				drawButton(options->buttons[i], 2);
			} else
				drawButton(options->buttons[i], 0);
		}
		for (i = 4; i < 8; i++) {
			drawButton(options->buttons[i], 0);
		}
	} else {
		for (i = 0; i < 4; i++) {
			if (options->option == i)
				drawButton(options->buttons[i], 1);
			else if (options->selectedCursor == i + 1)
				drawButton(options->buttons[i], 2);
			else
				drawButton(options->buttons[i], 0);
		}
		for (i = 4; i < 8; i++) {
			if (options->option == i)
				drawButton(options->buttons[i], 1);
			else
				drawButton(options->buttons[i], 0);
		}
	}
	int posX = 734, posY = 553;
	for (i = 0; i < 6; i++) {
		if (options->selectedSensitivity == i)
			drawRectangle(posX, posY, posX + 20, posY + 40, COLOUR_WHITE);
		posX += 55;
	}

}
void deleteOptions(Options* options) {
	deleteBitmap(options->background);
	int i;
	for (i = 0; i < 8; i++) {
		deleteButton(options->buttons[i]);
	}

	free(options);
}

//==========================================================================================
Credits* createCredits() {
	Credits* credits = (Credits*) malloc(sizeof(Credits));

	credits->background = loadBitmap((char*) getImagePath("credits"));
	credits->mouseSelection = -1;
	credits->option = -1;
	credits->done = 0;

	credits->button = createButton(535, 850, 745, 910, "mainMenuButton");

	return credits;
}
void updateCredits(Credits* credits) {
	if (getMouse()->draw)
		credits->option = -1;

	updateButton(credits->button);
	if (credits->button->onClick) {
		credits->mouseSelection = 0;
		resetMouseButton();
	}
}
void drawCredits(Credits* credits) {
	drawBitmap(credits->background, 0, 0, ALIGN_LEFT);
	if (getMouse()->draw)
		drawButton(credits->button, 0);
	else if (credits->option == 0)
		drawButton(credits->button, 1);
	else
		drawButton(credits->button, 0);
}
void deleteCredits(Credits* credits) {
	deleteBitmap(credits->background);
	deleteButton(credits->button);
	free(credits);
}
