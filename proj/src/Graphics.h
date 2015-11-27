#ifndef __GRAPHICS_H
#define __GRAPHICS_H

/** @defgroup graphics Graphics
 * @{
 * Functions for outputing data to screen in graphics mode
 */

/**
 * @brief Initializes the video module in graphics mode
 *
 * Uses the VBE INT 0x10 interface to set the desired
 *  graphics mode, maps VRAM to the process' address space and
 *  initializes static global variables with the resolution of the screen,
 *  and the number of colors
 *
 * @param mode 16-bit VBE mode to set
 * @return Virtual address VRAM was mapped to. NULL, upon failure.
 */
void *initGraphics(unsigned short mode);

 /**
 * @brief Returns to default Minix 3 text mode (0x03: 25 x 80, 16 colors)
 *
 * @return 0 upon success, non-zero upon failure
 */
int exitGraphics(void);

/**
 * @return Pointer to video_mem.
 */
char* getVideoMem();
/**
 * @return Horizontal resolution
 */
unsigned getHRes();
/**
 * @return Vertical resolution
 */
unsigned getVRes();

/**
 * Colors the screen with the specified color
 */
int fillDisplay(unsigned long color);
/**
 * Colors a pixel in the double buffer
 */
void putPixel(int x, int y, unsigned long color);
/**
 * Draws square with a given color at the specified position
 */
int drawSquare(int xi, int yi, int size, unsigned long color);
/**
 * Draws line with a given color in the specified coordinates
 */
int drawLine(int xi, int yi, int xf, int yf, unsigned long color);
/**
 * Draws a pixmap in the specified position.
 */
int drawPixmap(int xi, int yi, char* pixmap, int width, int height);
/**
 * Clears the pixmap position.
 */
int clearPixmap(int xi, int yi, int width, int height);

/** @} end of video_gr */


#endif /* __GRAPHICS_H */
