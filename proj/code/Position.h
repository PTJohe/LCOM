#ifndef __POSITION_H
#define __POSITION_H

typedef struct {
	int x;
	int y;
} Position;

Position* createPosition(int x, int y);
double calcDistance(Position* p1, Position* p2);
void deletePosition(Position* position);


#endif
