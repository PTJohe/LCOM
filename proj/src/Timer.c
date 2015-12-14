#include "Timer.h"
#include "Utilities.h"

#include <minix/syslib.h>
#include <minix/drivers.h>
#include <minix/com.h>
#include <stdio.h>
#include <stdlib.h>

int subscribeTimer(void) {
	hook_id_timer = TIMER0_IRQ;
	int bitmask = BIT(hook_id_timer); //TIMER0_IRQ = hook_id_timer = 0
	if (sys_irqsetpolicy(TIMER0_IRQ, IRQ_REENABLE, &hook_id_timer) == OK
			&& sys_irqenable(&hook_id_timer) == OK)
		return bitmask;
	else
		return EXIT_FAILURE;
}

int unsubscribeTimer() {
	if (sys_irqrmpolicy(&hook_id_timer) == OK
			&& sys_irqdisable(&hook_id_timer) == OK)
		return EXIT_SUCCESS;
	else
		return EXIT_FAILURE;
}

Timer* newTimer() {
	Timer* timer = (Timer*) malloc(sizeof(Timer));

	timer->counter = 0;
	timer->enabled = 0;

	return timer;
}

int getCount(Timer* timer) {
	return timer->counter;
}

void timerCount(Timer* timer) {
	timer->counter++;
	timer->enabled = 1;
}

void resetTimer(Timer* timer) {
	timer->counter = 0;
}

void startTimer (Timer* timer) {
	timer->enabled = 1;
}
void stopTimer(Timer* timer) {
	timer->enabled = 0;
}

int deleteTimer(Timer* timer) {
	free(timer);
	return EXIT_SUCCESS;
}

