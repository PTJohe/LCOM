#include <minix/sysutil.h>
#include <minix/syslib.h>
#include <minix/drivers.h>

#include "Utilities.h"
#include "WallyNIX.h"
#include "Graphics.h"
#include "Timer.h"
#include "Mouse.h"

int main(int argc, char *argv[]) {
	/* Initialize service (synchronize with the RS) */
	sef_startup();

	WallyNIX* wally = (WallyNIX*) startWallyNIX();
	while (!wally->exit) {
		updateWallyNIX(wally);

		if (!wally->exit) {
			if (wally->draw) {
				drawWallyNIX(wally);
			}
			if(getMouse()->draw){
				drawMouse();
			}
			copyMouseBuffer();
			copyDoubleBuffer();
		}
	}
	stopWallyNIX(wally);

	printf("WallyNIX exited successfully\n");
	return EXIT_SUCCESS;
}
