#include "pixmap.h"
#include <stdlib.h>

char **getPixmap(unsigned short i) {
	switch (i) {
	case 0:
		return pic1;
	case 1:
		return pic2;
	case 2:
		return cross;
	case 3:
		return pic3;
	case 4:
		return penguin;
	default:
		return NULL;
	}
}
