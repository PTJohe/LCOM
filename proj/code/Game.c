#include "Game.h"
#include "Utilities.h"
#include "Mouse.h"

#include <minix/syslib.h>
#include <minix/drivers.h>
#include <minix/com.h>
#include <stdio.h>
#include <stdlib.h>

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

void drawButton(Button* button, int highlight) {
	unsigned long colour = COLOUR_BLACK;

	if (getMouse()->draw == 1) {
		if (button->onHover == 1)
			colour = COLOUR_ORANGE;
	} else if (highlight == 1)
		colour = COLOUR_ORANGE;

	drawRectangle(button->topLeft->x, button->topLeft->y,
			button->bottomRight->x, button->bottomRight->y, colour);

	int x = (int) round((button->topLeft->x + button->bottomRight->x) / 2.0);
	int y = (int) round((button->topLeft->y + button->bottomRight->y) / 2.0)
			- 30;
	drawBitmap(button->text, x, y, ALIGN_CENTER);
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

void deleteButton(Button* button) {
	deletePosition(button->topLeft);
	deletePosition(button->bottomRight);
	deleteBitmap(button->text);
	free(button);
}

MainMenu* createMainMenu() {
	MainMenu* mainMenu = (MainMenu*) malloc(sizeof(MainMenu));

	mainMenu->background = loadBitmap((char*) getImagePath("mainMenu"));
	mainMenu->mouseSelection = 0;
	mainMenu->option = -1;
	mainMenu->done = 0;

	Button* arcadeMode = createButton(700, 300, 990, 370, "arcadeMode");
	Button* stageSelect = createButton(700, 380, 990, 450, "stageSelect");
	Button* highScores = createButton(700, 460, 990, 530, "highScores");
	Button* options = createButton(700, 540, 990, 610, "options");
	Button* exit = createButton(700, 620, 990, 690, "exitGame");

	mainMenu->buttons[0] = arcadeMode;
	mainMenu->buttons[1] = stageSelect;
	mainMenu->buttons[2] = highScores;
	mainMenu->buttons[3] = options;
	mainMenu->buttons[4] = exit;

	return mainMenu;
}

void updateMainMenu(MainMenu* mainMenu) {
	if (getMouse()->draw)
		mainMenu->option = -1;

	int i;
	for (i = 0; i < 5; i++) {
		updateButton(mainMenu->buttons[i]);
		if (mainMenu->buttons[i]->onClick) {
			mainMenu->mouseSelection = i + 1;
			resetMouseButton();
		}
	}
}

void drawMainMenu(MainMenu* mainMenu) {
	drawBitmap(mainMenu->background, 0, 0, ALIGN_LEFT);
	int i;
	if (getMouse()->draw) {
		for (i = 0; i < 5; i++) {
			drawButton(mainMenu->buttons[i], 0);
		}
	} else {
		for (i = 0; i < 5; i++) {
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
	for (i = 0; i < 5; i++) {
		deleteButton(mainMenu->buttons[i]);
	}
	free(mainMenu);
}

ArcadeMode* createArcadeMode() {
	ArcadeMode* arcadeMode = (ArcadeMode*) malloc(sizeof(ArcadeMode));

	arcadeMode->score = 0;
	arcadeMode->currentStage = 1;
	arcadeMode->found = 0;

	arcadeMode->pause = 0;
	arcadeMode->pauseContinue = createButton(515, 500, 805, 570,
			"pauseContinue");
	arcadeMode->pauseQuit = createButton(515, 580, 805, 650, "pauseQuit");

	arcadeMode->gameOver = 0;
	arcadeMode->done = 0;

	arcadeMode->findWally = loadBitmap(getImagePath("findWally"));
	arcadeMode->wally = loadBitmap(getImagePath("wally"));

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

		arcadeMode->stages[i] = createStage(stageNumber, position);
		i++;
	}
	fclose(file);

	return arcadeMode;
}

void updateArcadeMode(ArcadeMode* arcadeMode) {
	if (arcadeMode->found) {
		int waiting = arcadeMode->timer->counter - arcadeMode->found;
		if (waiting >= 1 * TIMER_DEFAULT_FREQ) {
			if (arcadeMode->currentStage == 10) {
				arcadeMode->done = 1;
				return;
			}
			arcadeMode->currentStage++;
			arcadeMode->found = 0;
			resetTimer(arcadeMode->timer);
			return;
		}
		return;
	}

	if (arcadeMode->pause) {
		drawRectangle(505, 490, 815, 660, COLOUR_GREY);
		updateButton(arcadeMode->pauseContinue);
		updateButton(arcadeMode->pauseQuit);

		if (arcadeMode->pauseContinue->onClick) {
			arcadeMode->pause = 0;
			resumeTimer(arcadeMode->timer);
		} else if (arcadeMode->pauseQuit->onClick) {
			arcadeMode->done = 1;
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

	if (stage->found) {
		drawCircleBorder(stage->wally->x, stage->wally->y, 25, COLOUR_ORANGE);
		arcadeMode->found = arcadeMode->timer->counter;

		int timeLeft = (60 - arcadeMode->timer->counter / TIMER_DEFAULT_FREQ);
		arcadeMode->score += 10 * timeLeft;

		LOG_VAR("Time Left", timeLeft);
		LOG_VAR("Score", arcadeMode->score);
	}

	if (arcadeMode->timer->counter >= 60 * TIMER_DEFAULT_FREQ) {
		arcadeMode->done = 1;
		stopTimer(arcadeMode->timer);
	}
}

void drawArcadeMode(ArcadeMode* arcadeMode) {
	if (arcadeMode->found) {
		return;
	}

	if (arcadeMode->pause) {
		drawButton(arcadeMode->pauseContinue, 0);
		drawButton(arcadeMode->pauseQuit, 0);
		return;
	}

	fillDisplay(COLOUR_WHITE);
	drawTimeLeft(arcadeMode->timer);

	drawBitmap(arcadeMode->findWally, 0, 30, ALIGN_LEFT);
	drawBitmap(arcadeMode->wally, 280, 10, ALIGN_LEFT);
	drawStage(arcadeMode->stages[arcadeMode->currentStage - 1]);
}

void deleteArcadeMode(ArcadeMode* arcadeMode) {
	int i;
	for (i = 0; i < 1 /*11*/; i++) {
		deleteStage(arcadeMode->stages[i]);
	}

	deleteButton(arcadeMode->pauseContinue);
	deleteButton(arcadeMode->pauseQuit);
	deleteBitmap(arcadeMode->findWally);
	deleteBitmap(arcadeMode->wally);
	deleteTimer(arcadeMode->timer);

	free(arcadeMode);
}

Stage* createStage(int stageNumber, Position* wally) {
	Stage* stage = (Stage*) malloc(sizeof(Stage));

	stage->timeLeft = 60;
	stage->found = 0;

	stage->pause = createButton(1050, 30, 1240, 100, "pause");

	stage->image = loadBitmap(getStagePath(stageNumber));
	stage->wally = wally;

	return stage;
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
		if (calcDistance(mouse->position, stage->wally) < 25) {
			stage->found = 1;
		}
	}
}

void drawStage(Stage* stage) {
	drawButton(stage->pause, 0);
	drawBitmap(stage->image, 0, 213, ALIGN_LEFT);
//drawCircle(stage->wally->x, stage->wally->y, 25, COLOUR_ORANGE);
}

void deleteStage(Stage* stage) {
	deleteButton(stage->pause);
	deleteBitmap(stage->image);
	deletePosition(stage->wally);
	free(stage);
}
