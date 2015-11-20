#ifndef __TIMER_H
#define __TIMER_H

static int timer_counter;
static int hook_id_timer;

#define TIMER_DEFAULT_FREQ 60 /**< @brief Default frequency of timer */

/**
 * @brief Subscribes and enables Timer 0 interrupts
 *
 * @return Returns bit order in interrupt mask; negative value on failure
 */
int timer_subscribe_int();

/**
 * @brief Unsubscribes Timer 0 interrupts
 *
 * @return Return 0 upon success and non-zero otherwise
 */
int timer_unsubscribe_int();

/**
 * @brief Timer 0 interrupt handler
 *
 * Increments counter
 */
void timer_int_handler();


#endif
