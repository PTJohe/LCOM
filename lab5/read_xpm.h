#ifndef __READ_XPM_H
#define __READ_XPM_H

/**
 * @brief Reads a xpm-like sprite defined in "map" (look at pixmap.h for
 * examples). Updates "width" and "heigh" with the sprite
 * dimension.
 *
 * @return Returns the address of the allocated memory where the
 * sprite was read. Return NULL on error.
 */
char *read_xpm(char *map[], int *wd, int *ht);

#endif
