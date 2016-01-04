#ifndef __GAME_H
#define __GAME_H

#include "Bitmap.h"
#include "Position.h"
#include "Timer.h"

/** @defgroup Game Game
 * @{
 * Functions for the game engine
 */

enum {
	MAIN_MENU,
	ARCADE_MODE,
	STAGE_SELECT,
	HIGH_SCORES,
	OPTIONS,
	EXIT_GAME,
	CREDITS,
};

/// Represents a Button
typedef struct {
	Position* topLeft;
	Position* bottomRight;
	Bitmap* text;

	int onHover;
	int onClick;
} Button;

/// Represents the Main Menu
typedef struct {
	Bitmap* background;

	Button* buttons[6];
	/*Button* arcadeMode;
	 Button* stageSelect;
	 Button* options;
	 Button* highScores;
	 Button* exit;
	 Button* credits*/

	int mouseSelection;
	int option;

	int done;
} MainMenu;

/// Represents a Stage
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

/// Represents the ArcadeMode
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
	int gameOver;
	int done;
} ArcadeMode;

/// Represents the StageSelect Menu
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

/// Represents a Score
typedef struct {
	int score;
	char* name;
	char* date;
} Score;

/// Represents the HighScores Menu
typedef struct {
	int draw;
	int mouseSelection;
	int option;
	int done;

	Score* scores[10];

	Bitmap* background;
	Button* button;

	int points;
	int inputName;
	int addScore;

	char input[11];
	Bitmap* newScore;
	Button* ok;
} HighScores;

/// Represents the Options Menu
typedef struct {
	int mouseSelection;
	int option;
	int done;

	int selectedCursor;
	int selectedSensitivity;

	Bitmap* background;
	Button* buttons[8];
} Options;

/// Represents the Credtis Menu
typedef struct {
	int mouseSelection;
	int option;
	int done;

	Bitmap* background;
	Button* button;
} Credits;

//==========================================================================================
/**
 * @brief Button constructor
 *
 * @param xi Top left y coordinate
 * @param yi Top left y coordinate
 * @param xf Bottom right x coordinate
 * @param yf Bottom right y coordinate
 * @param text Button image name
 *
 * @return Pointer to created button
 */
Button* createButton(int xi, int yi, int xf, int yf, char* text);
/**
 * @brief Checks is mouse is inside the button
 * @param button Pointer to button
 * @return 0 if false, 1 if true
 */
int mouseInButton(Button* button);
/**
 * @brief Updates button
 * @param button Pointer to button
 */
void updateButton(Button* button);
/**
 * @brief Draws button
 * @param button Pointer to button
 * @param menuOption Highlights button if it's the selected option
 */
void drawButton(Button* button, int menuOption);
/**
 * @brief Button destructor
 * @param button Pointer to button
 */
void deleteButton(Button* button);

//==========================================================================================
/**
 * @brief Main Menu constructor
 * @return Pointer to created main menu
 */
MainMenu* createMainMenu();
/**
 * @brief Updates main menu
 * @param mainMenu Pointer to main menu
 */
void updateMainMenu(MainMenu* mainMenu);
/**
 * @brief Draws main menu
 * @param mainMenu Pointer to main menu
 */
void drawMainMenu(MainMenu* mainMenu);
/**
 * @brief Main Menu destructor
 * @param mainMenu Pointer to main menu
 */
void deleteMainMenu(MainMenu* mainMenu);

//==========================================================================================
/**
 * @brief Arcade Mode constructor
 * @return Pointer to created arcade mode
 */
ArcadeMode* createArcadeMode();
/**
 * @brief Updates arcade mode
 * @param arcadeMode Pointer to arcade mode
 */
void updateArcadeMode(ArcadeMode* arcadeMode);
/**
 * @brief Draws arcade mode
 * @param arcadeMode Pointer to arcade mode
 */
void drawArcadeMode(ArcadeMode* arcadeMode);
/**
 * @brief Arcade Mode destructor
 * @param arcadeMode Pointer to arcade mode
 */
void deleteArcadeMode(ArcadeMode* arcadeMode);

//==========================================================================================
/**
 * @brief Stage Select constructor
 * @return Pointer to created stage select
 */
StageSelect* createStageSelect();
/**
 * @brief Updates stage select
 * @param stageSelect Pointer to stage select
 */
void updateStageSelect(StageSelect* stageSelect);
/**
 * @brief Selects stage to play
 * @param stageSelect Pointer to stage select
 * @param option Stage number
 */
void pickStageSelect(StageSelect* stageSelect, int option);
/**
 * @brief Returns to stage select menu
 * @param stageSelect Pointer to stage select
 */
void returnToStageSelect(StageSelect* stageSelect);
/**
 * @brief Dras stage select
 * @param stageSelect Pointer to stage select
 */
void drawStageSelect(StageSelect* stageSelect);
/**
 * @brief Stage Select destructor
 * @param stageSelect Pointer to stage select
 */
void deleteStageSelect(StageSelect* stageSelect);

//==========================================================================================
/**
 * @brief Creates stage for arcade mode
 *
 * @param stageNumber Stage number
 * @param wally Coordinates of wally
 *
 * @return Created stage
 */
Stage* createStageArcadeMode(int stageNumber, Position* wally);
/**
 * @brief Creates stage for stage select mode
 *
 * @param stageNumber Stage number
 * @param wally Coordinates of wally
 * @param wenda Coordinates of wenda
 * @param whitebeard Coordinates of whitebeard
 * @param odlaw Coordinates of odlaw
 *
 * @return Created stage
 */
Stage* createStage(int stageNumber, Position* wally, Position* wenda,
		Position* whitebeard, Position* odlaw);
/**
 * @brief Checks if player found all elements
 * @param stage Pointer to stage
 * @return 0 if false, 1 if true
 */
int foundAll(Stage* stage);
/**
 * @brief Updates stage
 * @param stage Pointer to stage
 */
void updateStage(Stage* stage);
/**
 * @brief Draws stage
 * @param stage Pointer to stage
 */
void drawStage(Stage* stage);
/**
 * @brief Resets stage flags
 * @param stage Pointer to stage
 */
void resetStage(Stage* stage);
/**
 * @brief Stage destructor
 * @param stage Pointer to stage
 */
void deleteStage(Stage* stage);

//==========================================================================================
/**
 * @brief HighScores constructor
 * @param score 0 if just viewing highscores, non-zero if player just finished arcade mode
 * @return Pointer to created High Scores
 */
HighScores* createHighScores(int score);
/**
 * @brief Sorts highscores
 * @param highScores Pointer to high scores
 */
void sortHighScores(HighScores* highScores);
/**
 * @brief Inserts score into the list of highscores
 * @param highScores Pointer to high scores
 * @param score Pointer to score
 */
void insertHighScore(HighScores* highScores, Score* score);
/**
 * @brief Updates highscores
 * @param highScores Pointer to high scores
 */
void updateHighScores(HighScores* highScores);
/**
 * @brief Inputs name for the highscore
 * @param highScores Pointer to high scores
 * @param scancode Keyboard input
 */
void inputName(HighScores* highScores, unsigned long scancode);
/**
 * @brief Draws name of the highscore
 * @param name Pointer to name
 * @param rank Position in highscores
 */
void drawName(char* name, int rank);
/**
 * @brief Draws points of the highscore
 * @param points Number of points
 * @param rank Position in highscores
 */
void drawScore(int points, int rank);
/**
 * @brief Draws date of the highscore
 * @param date Pointer to date
 * @param rank Position in highscores
 */
void drawDate(char* date, int rank);
/**
 * @brief Draws name input
 * @param highScores Pointer to high scores
 */
void drawInput(HighScores* highScores);
/**
 * @brief Draws high scores
 * @param highScores Pointer to high scores
 */
void drawHighScores(HighScores* highScores);
/**
 * @brief High Scores destructor
 * @param highScores Pointer to high scores
 */
void deleteHighScores(HighScores* highScores);

//==========================================================================================
/**
 * @brief Score constructor
 *
 * @param name Name
 * @param points Points
 * @param date Date
 *
 * @return Pointer to created score
 */
Score* createScore(char* name, int points, char* date);
/**
 * @brief Score destructor
 * @param score Pointer to score
 */
void deleteScore(Score* score);

//==========================================================================================
/**
 * @brief Options constructor
 * @return Pointer to created options
 */
Options* createOptions();
/**
 * @brief Updates options
 * @param options Pointer to options
 */
void updateOptions(Options* options);
/**
 * @brief Draws options
 * @param options Pointer to options
 */
void drawOptions(Options* options);
/**
 * @brief Options destructor
 * @param options Pointer to options
 */
void deleteOptions(Options* options);

//==========================================================================================
/**
 * @brief Credits constructor
 * @return Pointer to created credits
 */
Credits* createCredits();
/**
 * @brief Updates credits
 * @param credits Pointer to credits
 */
void updateCredits(Credits* credits);
/**
 * @brief Draws credits
 * @param credits Pointer to credits
 */
void drawCredits(Credits* credits);
/**
 * @brief Credits destructor
 * @param credits Pointer to credits
 */
void deleteCredits(Credits* credits);

/** @} end of Game */
#endif /* __GAME_H */
