#ifndef __WALLYNIX_H
#define __WALLYNIX_H

#include "Bitmap.h"
#include "Game.h"
#include "Timer.h"

/** @defgroup WallyNIX WallyNIX
 * @{
 * Functions to manipulate the game state and devices
 */

/// Represents the main game object
typedef struct WallyNIX {
	int IRQ_SET_TIMER;
	int IRQ_SET_KBD;
	int IRQ_SET_MOUSE;

	int menu;
	int exit;

	unsigned long scancode;
	Timer* timerFPS;

	MainMenu* mainMenu;
	ArcadeMode* arcadeMode;
	StageSelect* stageSelect;
	HighScores* highScores;
	Options* options;
	Credits* credits;

} WallyNIX;

/**
 * @brief Game constructor
 * @return Pointer to the created game
 */
WallyNIX* initWallyNIX();

/**
 * @brief Calls functions to update the game and the devices
 * @param wally Game pointer
 */
void updateWallyNIX(WallyNIX* wally);

/**
 * @brief Interruptions handlers
 * @param wally Game pointer
 */
void updateInterruptions(WallyNIX* wally);
/**
 * @brief Updates the game using the keyboard
 * @param wally Game pointer
 */
void updateKeyboard(WallyNIX* wally);
/**
 * @brief Updates game state
 * @param wally Game pointer
 */
void updateStates(WallyNIX* wally);

/**
 * @brief Draws the game
 * @param wally Game pointer
 */
void drawWallyNIX(WallyNIX* wally);
/**
 * @brief Calls all destructors and terminates the game
 * @param wally Game pointer
 */
void stopWallyNIX(WallyNIX* wally);

/** @} end of WallyNIX */
#endif /* __WALLYNIX_H */
