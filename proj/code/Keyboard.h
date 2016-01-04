#ifndef __KEYBOARD_H
#define __KEYBOARD_H

static int hook_id_keyboard;

/** @defgroup Keyboard Keyboard
 * @{
 * Functions to manipulate the keyboard
 */

// Keys
typedef enum {
	KEY_ESC = 0x0001,
	KEY_ENTER = 0x001C,
	KEY_SPACEBAR = 0x0039,
	KEY_BACKSPACE = 0x000E,

	KEY_Q = 0x0010,
	KEY_W = 0x0011,
	KEY_E = 0x0012,
	KEY_R = 0x0013,
	KEY_T = 0x0014,
	KEY_Y = 0x0015,
	KEY_U = 0x0016,
	KEY_I = 0x0017,
	KEY_O = 0x0018,
	KEY_P = 0x0019,
	KEY_A = 0x001E,
	KEY_S = 0x001F,
	KEY_D = 0x0020,
	KEY_F = 0x0021,
	KEY_G = 0x0022,
	KEY_H = 0x0023,
	KEY_J = 0x0024,
	KEY_K = 0x0025,
	KEY_L = 0x0026,
	KEY_Z = 0x002C,
	KEY_X = 0x002D,
	KEY_C = 0x002E,
	KEY_V = 0x002F,
	KEY_B = 0x0030,
	KEY_N = 0x0031,
	KEY_M = 0x0032,
} KEY;

/**
 * @brief Subscribe the keyboard interrupts
 * @return Return 0 upon success and non-zero otherwise
 */
int subscribeKeyboard();

/**
 * @brief Unsubscribe the keyboard interrupts
 * @return Return 0 upon success and non-zero otherwise
 */
int unsubscribeKeyboard();

/**
 * @brief Read KBC responses from Output Buffer
 * @param data Stores KBC Response (ACK, RESEND, ERROR)
 * @return Return 0 upon success and non-zero otherwise
 */
unsigned short read_data_OUTBUF_from_KBC(unsigned long* data);

/**
 * @brief Issues mouse command to KBC
 * @param command Stores mouse command
 * @return Return 0 upon success and non-zero otherwise
 */
unsigned short KBC_issue_command_mouse(unsigned char command);

/**
 * @brief Issues argument of any command to KBC
 * @param argument Stores KBC argument from any command
 * @return Return 0 upon success and non-zero otherwise
 */
unsigned short issue_argument_KBC(unsigned char argument);

/**
 * @brief Handle keyboard interrupts
 * Reads the scancode from the OUT_BUF
 * @return Return 0 upon success and non-zero otherwise
 */
unsigned long readScancode();

/** @} end of Keyboard */
#endif /* __KEYBOARD_H */
