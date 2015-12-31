#ifndef __STATE_H
#define __STATE_H

#include "Bitmap.h"

typedef struct {
	int xi, yi;
	int xf, yf;
	Bitmap* text;

	int onHover;
	int onClick;
} Button;

typedef struct {
	Bitmap* background;

	Button* buttons[5];
	/*Button* arcadeMode;
	Button* stageSelect;
	Button* options;
	Button* highScores;
	Button* exit;*/

	int mouseSelection;
} MainMenu;


typedef struct {
	int timeLeft;
	int score;

} ArcadeMode;

typedef struct {
	int timeLeft;

	Bitmap* image;
	Button* pause;

	//Wally coordinates
	int x, y;
} Stage;

Button* createButton(int xi, int yi, int xf, int yf, char* text);
void deleteButton(Button* button);

MainMenu* createMainMenu();
void deleteMainMenu(MainMenu* mainMenu);



#endif
