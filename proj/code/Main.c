#include <minix/sysutil.h>
#include <minix/syslib.h>
#include <minix/drivers.h>

#include "Utilities.h"
#include "WallyNIX.h"
#include "Graphics.h"
#include "Timer.h"
#include "Mouse.h"
#include "RTC.h"

FILE* logfd = NULL;

void initLog() {
	logfd = fopen(LOG_PATH, "w");
	LOG("initLog", "logging successfully initialized");
}
void exitLog() {
	fclose(logfd);
}

int main(int argc, char *argv[]) {
	initLog();
	LOG("main", "Starting Where's Wally? ...");

	/* Initialize service */
	sef_startup();

	/* Enable IO-sensitive operations for ourselves */
	sys_enable_iop(SELF);

	printf("Starting \"Where's Wally?\" ....\n");

	initGraphics(MODE_1280_1024);

	WallyNIX* wally = (WallyNIX*) startWallyNIX();
	while (!wally->exit) {
		updateWallyNIX(wally);

		if (!wally->exit) {
			if (wally->timerFPS->ticked) {
				wally->timerFPS->ticked = 0;
				drawWallyNIX(wally);

				copyToMouseBuffer();
				if (getMouse()->draw) {
					drawMouse();
				}

				copyToVideoMem();
			}
		}
	}
	stopWallyNIX(wally);

	exitGraphics();

	LOG("main", "Terminated Where's Wally? ...");
	printf("\"Where's Wally?\" exited successfully.\n");

	return EXIT_SUCCESS;
}
