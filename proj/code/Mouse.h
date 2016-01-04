#ifndef __MOUSE_H
#define __MOUSE_H

#include "Position.h"
#include "Bitmap.h"
#include <minix/sysutil.h>

static int hook_id_mouse = 12;

/** @defgroup Mouse Mouse
 * @{
 * Functions to manipulate the mouse
 */

// Represents a mouse
typedef struct {
	int x, y;
	Position* position;
	int xSign, ySign;
	int deltaX, deltaY;
	double sensitivity;

	int byteNumber;
	unsigned long packet[3];

	int leftButtonPressed;
	int leftButtonReleased;

	int cursor;
	Bitmap* cursorImage;

	int hasBeenUpdated;
	int draw;
} Mouse;

/**
 * @brief Subscribes mouse interrupts
 * @return 0 on success, non-zero otherwise
 */
int subscribeMouse();
/**
 * @brief Unsubscribes mouse interrupts
 * @return 0 on success, non-zero otherwise
 */
int unsubscribeMouse();
/**
 * @brief Enables mouse
 * @return 0 on success, non zero otherwise
 */
int enableMouse();

/**
 * @brief Read mouse register
 * @return 0 on success, non zero otherwise
 */
int readMouse(unsigned long* reg);

/**
 * @brief Writes to the KBC
 * @return 0 on success, non zero otherwise
 */
int writeToKBC(unsigned long port, unsigned char byte);
/**
 * @brief Writes to the mouse register
 * @return 0 on success, non zero otherwise
 */
int writeToMouse(unsigned char byte);

/**
 * @brief Returns mouse pointer
 * @return Mouse pointer
 */
Mouse* getMouse();
/**
 * @brief Mouse constructor
 * @return Pointer to the created struct
 */
Mouse* createMouse();
/**
 * @brief Updates mouse data
 */
void updateMouse();
/**
 * @brief Chances mouse cursor and sensitivity
 * @param cursor New cursor image
 * @param sensitivity New sensitivity
 */
void setMouseOptions(int cursor, int sensitivity);
/**
 * @brief Resets left button
 */
void resetMouseButton();
/**
 * @brief Draws mouse cursor to the mouse buffer
 */
void drawMouse();
/**
 * @brief Mouse destructor
 */
void deleteMouse();

/** @} end of Mouse */
#endif /* __MOUSE_H */
