#include "Position.h"

#include <math.h>

Position* createPosition(int x, int y) {
	Position* position = (Position*) malloc(sizeof(Position));

	position->x = x;
	position->y = y;

	return position;
}

double calcDistance(Position* p1, Position* p2) {
	return sqrt(pow((p2->x - p1->x), 2) + pow((p2->y - p1->y), 2));
}

void deletePosition(Position* position) {
	free(position);
}
