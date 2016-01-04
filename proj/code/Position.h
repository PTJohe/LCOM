#ifndef __POSITION_H
#define __POSITION_H

/** @defgroup Position Position
 * @{
 * Functions to manipulate a position
 */

/// Represents a position (x,y)
typedef struct {
	int x;
	int y;
} Position;

/**
 * @brief Create a position in the given coordinates
 *
 * @param x Coordenate X
 * @param y Coordenate Y
 * @return Pointer to the created position
 */
Position* createPosition(int x, int y);
/**
 * @brief Calculates distance between two positions
 *
 * @param p1 First position
 * @param p2 Second position
 * @return Distance value
 */
double calcDistance(Position* p1, Position* p2);

/**
 * @brief Position destructor
 * @param position Position to be deleted
 */
void deletePosition(Position* position);


/** @} end of Position */
#endif /* __POSITION_H */
