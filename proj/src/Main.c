#include <minix/sysutil.h>
#include <minix/syslib.h>
#include <minix/drivers.h>

#include "Utilities.h"
#include "WallyNIX.h"
#include "Graphics.h"
#include "Timer.h"

FILE* logfd = NULL;

void initLog(){
	logfd = fopen(LOG_PATH, "w");
	LOG("initLog", "logging successfully initialized");
}

int main(int argc, char *argv[]) {
	/* initialize log data */

	initLog();
	LOG("main", "Starting program..");

	/* Initialize service (synchronize with the RS) */
	sef_startup();

	WallyNIX* wally = (WallyNIX*) startWallyNIX();
	while (!wally->done) {
		updateWallyNIX(wally);

		if (!wally->done) {
			if (wally->draw) {
				drawWallyNIX(wally);
			}
		}
	}
	stopWallyNIX(wally);

	printf("WallyNIX exited successfully\n");
	return EXIT_SUCCESS;
}
