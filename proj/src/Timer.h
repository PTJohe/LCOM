#ifndef __TIMER_H
#define __TIMER_H

static int timer_counter;
static int hook_id_timer;

/// Represents a Timer
typedef struct {
	int counter; ///< timer counter
	int enabled; ///< bool indicating if timer is counting
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
Timer* newTimer();

/**
 * @brief Returns current count
 * @return int count
 */
int getCount(Timer* timer);

/**
 * @brief Timer 0 interrupt handler
 * Increments counter
 */
void timerCount(Timer* timer);
/**
 * @brief Resets timer counter to 0
 */
void resetTimer(Timer* timer);
/**
 * @brief Enables timer
 */
void startTimer (Timer* timer);
/**
 * @brief Disables timer
 */
void stopTimer(Timer* timer);
/**
 * @brief Deletes a timer
 *
 * @return Integer indicating success or failure
 */
int deleteTimer(Timer* timer);


#endif
