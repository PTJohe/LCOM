#ifndef __KEYBOARD_H
#define __KEYBOARD_H

/** @defgroup Keyboard Keyboard
 * @{
 * Functions to manipulate keyboard
 */

static int hook_id_keyboard;

#define KEY_DOWN(k) (k) //Checks if a certain key was pressed
#define KEY_UP(k)	(0x80 & k) //Checks if a certain key was released

/// Keys
typedef enum {
	KEY_ESC = 0x0001,
	KEY_W = 0x0011,
	KEY_A = 0x001E,
	KEY_S = 0x001F,
	KEY_D = 0x0020,
	KEY_ENTER = 0x001C,
	KEY_ARR_UP = 0xE048,
	KEY_ARR_LEFT = 0xE04B,
	KEY_ARR_RIGHT = 0xE04D,
	KEY_ARR_DOWN = 0xE050,
} KEY;

/**
 * @brief Subscribe the keyboard interrupts
 *
 * @return Return 0 upon success and non-zero otherwise
 */
int subscribeKeyboard();

/**
 * @brief Unsubscribe the keyboard interrupts
 *
 * @return Return 0 upon success and non-zero otherwise
 */
int unsubscribeKeyboard();

/**
 * @brief Handle keyboard interrupts
 *
 * Reads the scancode from the OUT_BUF
 *
 * @return Return 0 upon success and non-zero otherwise
 */
unsigned long readScancode();

#endif
