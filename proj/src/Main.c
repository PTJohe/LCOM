#include <minix/sysutil.h>
#include <minix/syslib.h>
#include <minix/drivers.h>

#include "Utilities.h"
#include "WallyNIX.h"
#include "Graphics.h"
#include "Timer.h"
#include "Mouse.h"

FILE* logfd = NULL;
void initLog() {
	logfd = fopen(LOG_PATH, "w");
	LOG("initLog", "logging successfully initialized");
}

int main(int argc, char *argv[]) {
	//initLog();
	//LOG("main", "Starting Where's Wally? ...");
	/* Initialize service (synchronize with the RS) */
	sef_startup();
	printf("jsadn");


	WallyNIX* wally = (WallyNIX*) startWallyNIX();
	while (!wally->exit) {
		updateWallyNIX(wally);

		if (!wally->exit) {
			if (wally->draw) {
				drawWallyNIX(wally);
			}
			if(getMouse()->draw){
				updateMouse();
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
