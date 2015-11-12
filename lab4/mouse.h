#ifndef __MOUSE_H
#define __MOUSE_H

#include <minix/syslib.h>
#include <minix/drivers.h>
#include <minix/com.h>
#include <stdio.h>
#include <stdlib.h>
#include <minix/sysutil.h>

static int hook_id_mouse = 12;
unsigned long packet[3];
static int byte_number = 0;


int mouse_subscribe_int();
int mouse_unsubscribe_int();
unsigned short send_argument_mouse(unsigned char argument);
unsigned short read_data_OUTBUF_from_MOUSE(unsigned long* data);
int mouse_enable_sending_packets();
int mouse_enable_stream_mode();
int mouse_disable_stream_mode();
unsigned long mouse_int_handler();
//unsigned long synchronize_bytes();


#endif
