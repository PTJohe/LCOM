#ifndef __KEYBOARD_H
#define __KEYBOARD_H

static int hook_id_keyboard = 1;

int kbd_subscribe_int();

int kbd_unsubscribe_int();

unsigned long keyboard_int_handler_C();

unsigned short read_data_OUTBUF_from_KBC(unsigned long* data);

unsigned short KBC_issue_command(unsigned char command);

unsigned short issue_argument_KBC(unsigned char argument);

//unsigned short issue_data_KBC(unsigned char data);

/*unsigned short issue_commandArgument_KBC(unsigned short command,
		unsigned short argument);*/

#endif
