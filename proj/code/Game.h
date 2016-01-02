#ifndef __GAME_H
#define __GAME_H

#include "Bitmap.h"
#include "Position.h"
#include "Timer.h"
enum {
	MAIN_MENU,
	ARCADE_MODE,
	STAGE_SELECT,
	HIGH_SCORES,
	OPTIONS,
	EXIT_GAME
};

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
	int option;

	int done;
} MainMenu;

typedef struct {
	int timeLeft;
	int found;

	Button* pause;

	Bitmap* image;
	Position* wally;
} Stage;

typedef struct {
	int score;
	Timer* timer;
	int found;

	int currentStage;
	Stage* stages[10];

	Bitmap* findWally;
	Bitmap* wally;

	int pause;
	Button* pauseContinue;
	Button* pauseQuit;

	int gameOver;
	int done;
} ArcadeMode;

Button* createButton(int xi, int yi, int xf, int yf, char* text);
void updateButton(Button* button);
void drawButton(Button* button, int menuOption);
void deleteButton(Button* button);

MainMenu* createMainMenu();
int getMainMenuOption(MainMenu* mainMenu);
void updateMainMenu(MainMenu* mainMenu);
void drawMainMenu(MainMenu* mainMenu);
void resetMainMenu(MainMenu* mainMenu);
void deleteMainMenu(MainMenu* mainMenu);

ArcadeMode* createArcadeMode();
void deleteArcadeMode(ArcadeMode* arcadeMode);

Stage* createStage(int stageNumber, Position* wally);
void updateStage(Stage* stage);
void drawStage(Stage* stage);
void deleteStage(Stage* stage);

#endif
