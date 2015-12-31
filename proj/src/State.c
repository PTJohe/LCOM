#include "State.h"

#include <minix/syslib.h>
#include <minix/drivers.h>
#include <minix/com.h>
#include <stdio.h>
#include <stdlib.h>

Button* createButton(int xi, int yi, int xf, int yf, char* text) {
	Button* button = (Button*) malloc(sizeof(Button));

	button->xi = xi;
	button->yi = yi;
	button->xf = xf;
	button->yf = yf;

	if (strcmp(text, "") == 0)
		button->text = NULL;
	else
		button->text = loadBitmap((char*)getImagePath(text));

	button->onHover = 0;
	button->onClick = 0;

	return button;
}
void deleteButton(Button* button) {
	deleteBitmap(button->text);
	free(button);
}

MainMenu* createMainMenu() {
	MainMenu* mainMenu = (MainMenu*) malloc(sizeof(MainMenu));

	mainMenu->background = loadBitmap((char*)getImagePath("mainMenu"));
	mainMenu->mouseSelection = 0;

	Button* arcadeMode = createButton(700, 360, 900, 440, "arcadeMode");
	Button* stageSelect = createButton(700, 470, 900, 550, "stageSelect");
	Button* highScores = createButton(700, 690, 900, 770, "highScores");
	Button* options = createButton(700, 580, 900, 660, "options");
	Button* exit = createButton(700, 800, 900, 890, "exit");

	mainMenu->buttons[0] = arcadeMode;
	mainMenu->buttons[1] = stageSelect;
	mainMenu->buttons[2] = highScores;
	mainMenu->buttons[3] = options;
	mainMenu->buttons[4] = exit;

	return mainMenu;
}
void deleteMainMenu(MainMenu* mainMenu) {
	int i;
	for (i = 0; i < 5; i++) {
		deleteButton(mainMenu->buttons[i]);
	}
	free(mainMenu);
}
