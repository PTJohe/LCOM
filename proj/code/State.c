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

	LOG_VAR("OnClick", button->onClick);
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
	int timeLeft = 60;
	int score = 0;

	int currentStage = 1;

	/*FILE *file;
	char line[10];
	size_t len = 0;
	int i = 0;

	file = fopen("/home/lcom/lcom1516-t2g15/proj/res/stages/coordinates.txt",
			"r");
	while (getline(&line, &len, file) != -1) {
		int stageNumber = atoi(line);
		getline(&line, &len, file);

		int x = atoi(line);
		getline(&line, &len, file);
		int y = atoi(line);
		Position* position = createPosition(x, y);

		arcadeMode->stages[i] = createStage(stageNumber, position);
		i++;
	}
	fclose(file);*/

	int pause = 0;
	int gameOver = 0;
	int done = 0;

	return arcadeMode;
}
void deleteArcadeMode(ArcadeMode* arcadeMode) {

}

Stage* createStage(int stageNumber, Position* wally) {
	Stage* stage = (Stage*) malloc(sizeof(Stage));

	stage->timeLeft = 60;

	stage->image = loadBitmap(getStagePath(stageNumber));
	stage->wally = wally;

	return stage;
}
void deleteStage(Stage* stage) {

}
