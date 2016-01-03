#include "RTC.h"

#include "Utilities.h"
#include "minix/driver.h"
#include <minix/syslib.h>
#include <minix/drivers.h>
#include <minix/com.h>
#include <stdio.h>
#include <stdlib.h>

int subscribeRTC() {
	hook_id_rtc = RTC_IRQ;
	int bitmask = BIT(hook_id_rtc);

	if ((sys_irqsetpolicy(RTC_IRQ, IRQ_REENABLE, &hook_id_rtc) == OK)
			|| (sys_irqenable(&hook_id_rtc) == OK))
		return bitmask;
	else
		return EXIT_FAILURE;
}

int unsubscribeRTC() {
	if (sys_irqrmpolicy(&hook_id_rtc) == OK
			&& sys_irqdisable(&hook_id_rtc) == OK)
		return EXIT_SUCCESS;
	else
		return EXIT_FAILURE;
}

short readRTC(long array) {

	long temp;
	short ret;

	sys_outb(RTC_ADDR_REG, array);
	sys_inb(RTC_DATA_REG, &temp);

	ret = (short) temp;
	return ret;
}

char* getDateRTC() {
	if (subscribeRTC() == -1) {
		LOG("ERROR", "Failed to subscribe RTC!");
		return NULL;
	}

	char date[256];

	while (1) {
		if ((BIT(7) & readRTC(10)) == BIT(7)) {

			sprintf(date, "%02x/%02x/%x %02x:%02x:%02x\n", readRTC(7),
					readRTC(8), readRTC(9), readRTC(4), readRTC(2), readRTC(0));
			//printf("%s", date);
			LOG("Date", date);

			unsubscribeRTC();

			char* str = (char*) malloc(256);
			strcpy(str, date);
			return str;
		}
	}

	unsubscribeRTC();
	return NULL;
}
