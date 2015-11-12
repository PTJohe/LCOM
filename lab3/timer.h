#ifndef __TIMER_H
#define __TIMER_H

static int timer_counter;
static int hook_id_timer;

int timer_subscribe_int(void);

int timer_unsubscribe_int();

void timer_int_handler();

#endif
