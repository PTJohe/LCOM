#ifndef __KEYBOARD_H
#define __KEYBOARD_H


static int hook_id_keyboard = 1;

int kbd_subscribe_int();

int kbd_unsubscribe_int();

unsigned long keyboard_int_handler_C();

unsigned long issue_data_KBC(unsigned char data);

unsigned long issue_commandArgument_KBC(unsigned char command,
		unsigned long argument);

#endif
