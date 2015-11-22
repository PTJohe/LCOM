#ifndef __KEYBOARD_H
#define __KEYBOARD_H

static int hook_id_keyboard;

/**
 * @brief Subscribe the keyboard interrupts
 *
 * @return Return 0 upon success and non-zero otherwise
 */
int kbd_subscribe_int();

/**
 * @brief Unsubscribe the keyboard interrupts
 *
 * @return Return 0 upon success and non-zero otherwise
 */
int kbd_unsubscribe_int();
int kbd_scan_c(int *apt);
/**
 * @brief Handle keyboard interrupts
 *
 * Reads the scancode from the OUT_BUF
 *
 * @return Return 0 upon success and non-zero otherwise
 */
unsigned long keyboard_int_handler_C();

/**
 * @bried Reads keyboard's outbuf data
 *
 * @return Return 0 upon success and non-zero otherwise
 */
unsigned short read_data_OUTBUF_from_KBC(unsigned long* data);

/**
 * @brief Issues mouse command
 *
 * @return Return 0 upon success and non-zero otherwise
 */
unsigned short KBC_issue_command_mouse(unsigned char command);

/**
 * @brief Issues keyboard argument
 *
 * @return Return 0 upon success and non-zero otherwise
 */
unsigned short issue_argument_KBC(unsigned char argument);

#endif
