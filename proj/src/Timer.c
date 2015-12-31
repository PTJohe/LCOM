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

Timer* createTimer() {
	Timer* timer = (Timer*) malloc(sizeof(Timer));

	timer->counter = 60 * TIMER_DEFAULT_FREQ;
	timer->enabled = 0;

	int i;
	for (i = 0; i < 10; i++) {
		char str[256];
		sprintf(str, "%d", i);

		timer->numbers[i] = loadBitmap(getFontPath(str));
	}
	return timer;
}

int getCount(Timer* timer) {
	return timer->counter;
}

void timerCount(Timer* timer) {
	timer->counter--;
	timer->enabled = 1;
}

void resetTimer(Timer* timer) {
	timer->counter = 60 * TIMER_DEFAULT_FREQ;
}

void startTimer(Timer* timer) {
	timer->enabled = 1;
}
void stopTimer(Timer* timer) {
	timer->enabled = 0;
}

void drawTimeLeft(Timer* timer) {
	int number = (int) round(timer->counter / TIMER_DEFAULT_FREQ);
	int numDigits = 0;
	if (number < 10)
		numDigits = 1;
	else
		numDigits = 2;

	LOG_VAR("Counter", timer->counter);
	LOG_VAR("Default freq", TIMER_DEFAULT_FREQ);
	LOG_VAR("Number", number);

	if (numDigits == 1) {
		drawBitmap(timer->numbers[number], 640, 50, ALIGN_CENTER);
	} else if (numDigits == 2) {
		int secondDigit = number % 10;
		number /= 10;
		int firstDigit = number % 10;

		drawBitmap(timer->numbers[firstDigit], 610, 50, ALIGN_CENTER);
		drawBitmap(timer->numbers[secondDigit], 660, 50, ALIGN_CENTER);
	}
}

void deleteTimer(Timer* timer) {
	int i;
	for (i = 0; i < 10; i++) {
		deleteBitmap(timer->numbers[i]);
	}
	free(timer);
}

