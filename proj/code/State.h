#ifndef __STATE_H
#define __STATE_H

#include "Bitmap.h"
#include "Position.h"

typedef struct {
	Position* topLeft;
	Position* bottomRight;
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
	int done;
} MainMenu;

typedef struct {
	int timeLeft;

	Button* pause;
	Bitmap* image;

	//Wally coordinates
	Position* wally;
} Stage;

typedef struct {
	int timeLeft;
	int score;

	int currentStage;
	Stage* stages[11];

	int pause;
	int gameOver;
	int done;
} ArcadeMode;


Button* createButton(int xi, int yi, int xf, int yf, char* text);
void updateButton(Button* button);
void drawButton(Button* button);
void deleteButton(Button* button);

MainMenu* createMainMenu();
void drawMainMenu(MainMenu* mainMenu);
void resetMainMenu(MainMenu* mainMenu);
void deleteMainMenu(MainMenu* mainMenu);

ArcadeMode* createArcadeMode();
void deleteArcadeMode(ArcadeMode* arcadeMode);

Stage* createStage(int stageNumber, Position* wally);
void deleteStage(Stage* stage);



#endif
