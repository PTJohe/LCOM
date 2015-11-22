#include "sprite.h"
#include "read_xpm.h"
#include <stdlib.h>

Sprite * create_sprite(char *pic[], char *base) {
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

static int draw_sprite(Sprite *sp, char *base) {
//...
}
