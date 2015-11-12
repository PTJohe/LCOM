#ifndef __TEST4_H
#define __TEST4_H

#include <minix/syslib.h>
#include <minix/drivers.h>
#include <minix/com.h>
#include <stdio.h>
#include <stdlib.h>
#include <minix/sysutil.h>

#include "mouse.h"
#include "timer.h"
#include "keyboard.h"
#include "i8042.h"

typedef enum {INIT, DRAW, COMP} state_t;
typedef enum {RDOW, RUP, MOVE} ev_type_t;

/** @defgroup test4 test4
 * @{
 *
 * Functions for testing the kbd code
 */

/**
 *@brief To test packet handler
 *
 *
 *
 * @param cnt Number of packets to receive and display before exiting
 */

unsigned long packet_int_handler();

/**
 * @brief To test async handler
 *
 *
 */

unsigned long async_int_handler();

/**
 * @brief To test packet reception via interrupts
 *
 * Displays the packets received from the mouse
 * Exits after receiving the number of packets received in argument
 *
 * @param cnt Number of packets to receive and display before exiting
 *
 * @return Return 0 upon success and non-zero otherwise
 */

int test_packet(unsigned short cnt);

/**
 * @brief To test handling of more than one interrupt
 *
 *  Similar test_packet() except that it
 *  should terminate if no packets are received for idle_time seconds
 *
 * @param idle_time Number of seconds without packets before exiting
 *
 * @return Return 0 upon success and non-zero otherwise
 */

int test_async(unsigned short idle_time);

/**
 * @brief To test reading and parsing of configuration
 *
 *  Reads mouse configuration and displays it in a human-friendly way
 *
 * @return Return 0 upon success and non-zero otherwise
 */
int test_config(void);

/**
 * @brief to check if line exceeds specific horizontal tolerance
 *
 * @param structure event_t which stores events type
 *
 * @return Return 0 upon success and non-zero otherwise
 */

unsigned long check_hor_line(ev_type_t evt, short tolerance, short length, int deltaX, int deltaY);

/**
 * @brief To test_gesture handler
 *
 */

unsigned long gesture_int_handler(short length, unsigned short tolerance);

/**
 * @brief To test state machine implementation
 *
 *  Similar test_packet() except that it
 *  should terminate if user moves the mouse continuously in one
 *    vertical direction while pressing the right key down
 *
 * @param length minimum length of movement (mouse units).
 *  If positive, upwards movement, otherwise downwards.
 *
 * @param tolerance maximum movement (mouse units) on either horizontal
 * 		direction required to consider a movement as "vertical"
 *
 * @return Return 0 upon success and non-zero otherwise
 */


int test_gesture(short length, unsigned short tolerance);

#endif /* __TEST4_H */
