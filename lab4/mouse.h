#ifndef __MOUSE_H
#define __MOUSE_H

#include "i8042.h"

static int hook_id_mouse = 12;
unsigned long packet[3];
int byte_number = 0;


int mouse_subscribe_int();
int mouse_unsubscribe_int();
unsigned short send_argument_mouse(unsigned char argument);
int mouse_enable_sending_packets();
int mouse_enable_stream_mode();
unsigned long mouse_int_handler();


#endif
