#ifndef __GAME_H
#define __GAME_H

#include "Bitmap.h"
#include "Position.h"
#include "Timer.h"
enum {
	MAIN_MENU, ARCADE_MODE, STAGE_SELECT, HIGH_SCORES, OPTIONS, EXIT_GAME
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
	int arcadeMode;

	Button* pause;
	Bitmap* image;

	int foundWally;
	int foundWenda;
	int foundWhitebeard;
	int foundOdlaw;

	Position* wally;
	Position* wenda;
	Position* whitebeard;
	Position* odlaw;
} Stage;

typedef struct {
	int score;
	Timer* timer;
	int foundWally;

	int currentStage;
	Stage* stages[9];

	Bitmap* checkmark;
	Bitmap* findWally;
	Bitmap* wally;

	int pause;
	Button* pauseContinue;
	Button* pauseQuit;

	int option;
	int done;
} ArcadeMode;

typedef struct {
	int foundAll;
	int currentStage;
	int pause;
	int mouseSelection;
	int option;
	int done;

	Timer* timer;

	Bitmap* background;
	Button* buttons[10];

	Bitmap* checkmark;

	Bitmap* findWally;
	Bitmap* wally;
	Bitmap* findWenda;
	Bitmap* wenda;
	Bitmap* findWhitebeard;
	Bitmap* whitebeard;
	Bitmap* findOdlaw;
	Bitmap* odlaw;

	Stage* stages[9];
	Button* pauseContinue;
	Button* pauseQuit;

} StageSelect;

typedef struct {
	int score;
	char* name;
	char* date;
} Score;

typedef struct {
	int drawFirstTime;
	int mouseSelection;
	int option;
	int done;
	int newScore;

	Score* scores[10];

	Bitmap* background;
	Button* button;
} HighScores;


typedef struct {
	int mouseSelection;
	int option;
	int done;

	int selectedCursor;
	int selectedSensitivity;

	Bitmap* background;
	Button* buttons[8];
} Options;

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
void updateArcadeMode(ArcadeMode* arcadeMode);
void drawArcadeMode(ArcadeMode* arcadeMode);
void deleteArcadeMode(ArcadeMode* arcadeMode);

StageSelect* createStageSelect();
void updateStageSelect(StageSelect* stageSelect);
void pickStageSelect(StageSelect* stageSelect, int option);
void returnToStageSelect(StageSelect* stageSelect);
void drawStageSelect(StageSelect* stageSelect);
void deleteStageSelect(StageSelect* stageSelect);

Stage* createStageArcadeMode(int stageNumber, Position* wally);
Stage* createStage(int stageNumber, Position* wally, Position* wenda,
		Position* whitebeard, Position* odlaw);
void updateStage(Stage* stage);
void drawStage(Stage* stage);
void resetStage(Stage* stage);
void deleteStage(Stage* stage);

HighScores* createHighScores(int score);
void insertHighScore(HighScores* highScores, Score* score);
void sortHighScores(HighScores* highScores);
void drawName(char* name, int rank);
void drawScore(int score, int rank);
void drawDate(char* date, int rank);
void drawHighScores(HighScores* highScores);
void deleteHighScores(HighScores* highScores);

Score* createScore(char* name, int points, char* date);
void deleteScore(Score* score);

Options* createOptions();
void updateOptions(Options* options);
void drawOptions(Options* options);
void deleteOptions(Options* options);

#endif
