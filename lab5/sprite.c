#include "sprite.h"
#include "read_xpm.h"
#include <stdlib.h>

Sprite * create_sprite(char *pic[], int xi, int yi) {
	//allocate space for the "object"
	Sprite *sp = (Sprite *) malloc(sizeof(Sprite));
	if (sp == NULL)
		return NULL;
	// read the sprite pixmap
	sp->map = read_xpm(pic, &(sp->width), &(sp->height));
	if (sp->map == NULL) {
		free(sp);
		return NULL;
	}
	sp->x = xi;
	sp->y = yi;
	return sp;
}

void destroy_sprite(Sprite *sp, char *base) {
	if (sp == NULL)
		return;
	free(sp->map);
	free(sp);
	sp = NULL;
}

int animate_sprite(Sprite *sp, char *base) {
//...
}

static int check_collision(Sprite *sp, char *base) {
//...
}
void move_cursor(Sprite *sp, int xstep, int ystep, char *base) {
	//...
}
