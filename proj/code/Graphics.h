#ifndef __GRAPHICS_H
#define __GRAPHICS_H

/** @defgroup Graphics Graphics
 * @{
 * Functions for outputing data to screen in graphics mode
 */

/**
 * @brief Initializes the video module in graphics mode
 * @param mode 16-bit VBE mode to set
 * @return Virtual address VRAM was mapped to. NULL, upon failure.
 */
void *initGraphics(unsigned short mode);

/**
 * @brief Returns to default Minix 3 text mode (0x03: 25 x 80, 16 colors)
 * @return 0 upon success, non-zero upon failure
 */
int exitGraphics(void);

/**
 * @return Pointer to videoMem.
 */
char* getVideoMem();
/**
 * @return Pointer to doubleBuffer.
 */
char* getDoubleBuffer();

/**
 * @return Pointer to mouseBuffer
 */
char* getMouseBuffer();

/**
 * @return Horizontal resolution
 */
unsigned getHRes();
/**
 * @return Vertical resolution
 */
unsigned getVRes();
/**
 * @brief Copies the double buffer into the mouse buffer.
 */
void copyToMouseBuffer();
/**
 * @brief Copies the mouse buffer into the video mem.
 */
void copyToVideoMem();

//==========================================================================================
/**
 * @brief Colors the screen with the specified color
 * @param colour Colour to be painted
 * @return 0 on success, non-zero otherwise
 */
int fillDisplay(unsigned long colour);
/**
 * @brief Colors a pixel in the double buffer
 *
 * @param x Coordinate X
 * @param y Coordinate Y
 * @param colour Colour for the pixel
 */
void putPixel(int x, int y, unsigned long colour);

/**
 * @brief Draws a rectangle with a given color at the specified coordinates.
 *
 * @param xi Top left x coordinate
 * @param yi Top left y coordinate
 * @param xf Bottom right x coordinate
 * @param yf Bottom right y coordinate
 * @param colour Colour to fill the rectangle
 *
 * @return 0 on success, non-zero otherwise
 */
int drawRectangle(int xi, int yi, int xf, int yf, unsigned long colour);
/**
 * @brief Draws square with a given color at the specified position
 *
 * @param xi Top left x coordinate
 * @param yi Top left y coordinate
 * @param size Size of the square
 * @param colour Colour to fill the square
 *
 * @return 0 on success, non-zero otherwise
 */
int drawSquare(int xi, int yi, int size, unsigned long colour);
/**
 * @brief Draws line with a given color in the specified coordinates
 *
 * @param xi Starting x coordinate
 * @param yi Starting y coordinate
 * @param xf Ending x coordinate
 * @param yf Ending y coordinate
 * @param colour Colour to draw the line
 *
 * @return 0 on success, non-zero otherwise
 */
int drawLine(int xi, int yi, int xf, int yf, unsigned long colour);

/**
 * @brief Draws a filled circle with a given color and radius in the specified coordinates
 *
 * @param xc Center x coordinate
 * @param yc Center y cooridnate
 * @param radius Radius of the circle
 * @param colour Colour to fill the circle
 *
 * @return 0 on success, non-zero otherwise
 */
int drawCircle(int xc, int yc, int radius, unsigned long colour);
/**
 * @brief Draws the circle borders with a given color and radius in the specified coordinates
 *
 * @param xc Center x coordinate
 * @param yc Center y cooridnate
 * @param radius Radius of the circle
 * @param colour Colour to draw the borders
 *
 * @return 0 on success, non-zero otherwise
 */
int drawCircleBorder(long xc, long yc, int radius, unsigned long color);

/**
 * @brief Draws a pixmap in the specified position.
 *
 * @param xi X coordinate
 * @param yi Y coordinate
 * @param pixmap Pixmap to be drawn
 * @param width Pixmap width
 * @param height Pixmap height
 *
 * @return 0 on success, non-zero otherwise
 */
int drawPixmap(int xi, int yi, char* pixmap, int width, int height);
/**
 * @brief Clears the pixmap position.
 *
 * @param xi X coordinate
 * @param yi Y coordinate
 * @param width Pixmap width
 * @param height Pixmap height
 *
 * @return 0 on success, non-zero otherwise
 */
int clearPixmap(int xi, int yi, int width, int height);

/** @} end of Graphics */
#endif /* __GRAPHICS_H */
