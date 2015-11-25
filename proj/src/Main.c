#include <minix/drivers.h>

#include "Graphics.h"
#include "Keyboard.h"
#include "Mouse.h"
#include "Utilities.h"
#include "Timer.h"



int main(int argc, char *argv[]) {
	/* Initialize service (synchronize with the RS) */
	sef_startup();
	printf("OK\n");
	return EXIT_SUCCESS;
}
