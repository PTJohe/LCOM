#include "State.h"
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
		button->text = loadBitmap((char*) getImagePath(text));

	button->onHover = 0;
	button->onClick = 0;

	return button;
}

void drawButton(Button* button) {
	unsigned long colour = COLOUR_BLACK;
	if (button->onHover == 1)
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
	mainMenu->done = 0;

	Button* arcadeMode = createButton(700, 360, 1000, 440, "arcadeMode");
	Button* stageSelect = createButton(700, 470, 1000, 550, "stageSelect");
	/*Button* highScores = createButton(700, 690, 1000, 770, "highScores");
	 Button* options = createButton(700, 580, 1000, 660, "options");
	 Button* exit = createButton(700, 800, 1000, 890, "exit");*/

	mainMenu->buttons[0] = arcadeMode;
	mainMenu->buttons[1] = stageSelect;
	/*mainMenu->buttons[2] = highScores;
	 mainMenu->buttons[3] = options;
	 mainMenu->buttons[4] = exit;*/

	return mainMenu;
}

void drawMainMenu(MainMenu* mainMenu) {
	drawBitmap(mainMenu->background, 0, 0, ALIGN_LEFT);
	int i;
	for (i = 0; i < 2; i++) {
		drawButton(mainMenu->buttons[i]);
	}
}

void updateMainMenu(MainMenu* mainMenu) {
	int i;
	for (i = 0; i < 2; i++) {
		updateButton(mainMenu->buttons[i]);
		if (mainMenu->buttons[i]->onClick == 1) {
			mainMenu->mouseSelection = i + 1;
			resetMouseButton();
		}
	}
}

void resetMainMenu(MainMenu* mainMenu) {
	mainMenu->done = 0;
	mainMenu->mouseSelection = 0;
}

void deleteMainMenu(MainMenu* mainMenu) {
	deleteBitmap(mainMenu->background);

	/*int i;
	 for (i = 0; i < 5; i++) {
	 deleteButton(mainMenu->buttons[i]);
	 }*/
	free(mainMenu);
}

ArcadeMode* createArcadeMode() {
	ArcadeMode* arcadeMode = (ArcadeMode*) malloc(sizeof(ArcadeMode));

	arcadeMode->timeLeft = 60;
	arcadeMode->score = 0;

	arcadeMode->timer = createTimer();

	arcadeMode->currentStage = 1;

	FILE *file = fopen(PATH_STAGE "Coordinates.txt", "r");
	if (file == NULL)
		LOG("ERROR", "Couldn't open coordinates file.");

	int i = 0;
	char string[100];
	while (fgets(string, 100, file)) {
		int stageNumber = atoi(string);

		fgets(string, 100, file);
		int x = atoi(string);

		fgets(string, 100, file);
		int y = atoi(string);

		Position* position = createPosition(x, y + 213);

		arcadeMode->stages[i] = createStage(stageNumber, position);
		i++;
	}

	fclose(file);

	arcadeMode->pause = 0;
	arcadeMode->gameOver = 0;
	arcadeMode->done = 0;

	return arcadeMode;
}

void updateArcadeMode(ArcadeMode* arcadeMode) {
	Stage* stage = arcadeMode->stages[arcadeMode->currentStage - 1];
	updateStage(stage);

	if (stage->found) {
		arcadeMode->done = 1;
		stopTimer(arcadeMode->timer);
		return;
	}

	if (arcadeMode->timer->counter == 60 * TIMER_DEFAULT_FREQ) {
		arcadeMode->done = 1;
		stopTimer(arcadeMode->timer);
	}
}

void drawArcadeMode(ArcadeMode* arcadeMode) {
	fillDisplay(COLOUR_WHITE);
	drawTimeLeft(arcadeMode->timer);

	drawStage(arcadeMode->stages[arcadeMode->currentStage - 1]);
}

void deleteArcadeMode(ArcadeMode* arcadeMode) {
	int i;
	for (i = 0; i < 1 /*11*/; i++) {
		deleteStage(arcadeMode->stages[i]);
	}

	deleteTimer(arcadeMode->timer);
	free(arcadeMode);
}

Stage* createStage(int stageNumber, Position* wally) {
	Stage* stage = (Stage*) malloc(sizeof(Stage));

	stage->timeLeft = 60;
	stage->found = 0;

	stage->image = loadBitmap(getStagePath(stageNumber));
	stage->wally = wally;

	return stage;
}

void updateStage(Stage* stage) {
	Mouse* mouse = getMouse();

	if (mouse->leftButtonReleased == 1) {
		if (calcDistance(mouse->position, stage->wally) < 25) {
			stage->found = 1;
		}
	}
}

void drawStage(Stage* stage) {
	drawBitmap(stage->image, 0, 213, ALIGN_LEFT);
	drawCircle(stage->wally->x, stage->wally->y, 25, COLOUR_ORANGE);
}

void deleteStage(Stage* stage) {
//deleteButton(stage->pause);
	deleteBitmap(stage->image);
	deletePosition(stage->wally);
	free(stage);
}
