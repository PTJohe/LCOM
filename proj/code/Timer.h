#ifndef __TIMER_H
#define __TIMER_H

static int timer_counter;
static int hook_id_timer;

#include "Bitmap.h"

/** @defgroup Timer Timer
 * @{
 * Functions to manipulate a timer
 */

/// Represents a Timer
typedef struct {
	int counter;
	int enabled;
	int ticked;

	Bitmap* numbers[10];
} Timer;

/**
 * @brief Subscribes and enables Timer 0 interrupts
 *
 * @return Returns bit order in interrupt mask; negative value on failure
 */
int subscribeTimer();

/**
 * @brief Unsubscribes Timer 0 interrupts
 *
 * @return Return 0 upon success and non-zero otherwise
 */
int unsubscribeTimer();

/**
 * @brief Creates a timer
 * @return Non NULL pointer to Timer_t
 */
Timer* createTimer();

/**
 * @brief Returns current count
 *
 * @param timer Pointer to timer
 * @return int count
 */
int getCount(Timer* timer);

/**
 * @brief Timer 0 interrupt handler, increments counter
 * @param timer Pointer to timer
 */
void timerCount(Timer* timer);
/**
 * @brief Resets timer counter to 0
 * @param timer Pointer to timer
 */
void resetTimer(Timer* timer);
/**
 * @brief Enables timer
 * @param timer Pointer to timer
 */
void startTimer(Timer* timer);
/**
 * @brief Disables timer
 * @param timer Pointer to timer
 */
void stopTimer(Timer* timer);
/**
 * @brief Resumes timer count
 * @param timer Pointer to timer
 */
void resumeTimer(Timer* timer);
/**
 * @brief Draws the timer countdown
 * @param timer Pointer to timer
 * @param timeLimit Starting number
 */
void drawTimeLeft(Timer* timer, int timeLimit);
/**
 * @brief Deletes a timer
 * @param timer Pointer to timer
 */
void deleteTimer(Timer* timer);

/** @} end of Timer */
#endif /* __TIMER_H */
