#include "mouse.h"
#include "keyboard.h"

#include <minix/drivers.h>

int mouse_subscribe_int(){
	int bitmask = BIT(hook_id_mouse);
	if (sys_irqsetpolicy(MOUSE_IRQ, IRQ_REENABLE | IRQ_EXCLUSIVE, &hook_id_mouse) == OK)
		/*memset(packet, 0, sizeof(packet));
		byte_number = 0;*/ // set all bytes bit 3 to 0 and byte_number (index of byte in packet of 3 bytes) to 0
		return bitmask;
	else
		return EXIT_FAILURE;
}

int mouse_unsubscribe_int(){
	if (sys_irqdisable(&hook_id_mouse) == OK && sys_irqrmpolicy(&hook_id_mouse) == OK)
		return EXIT_SUCCESS;
	else
		return EXIT_FAILURE;
}

unsigned short send_argument_mouse(unsigned char argument){
	unsigned long KBC_value;
	if (issue_command_KBC(WRITE_BYTE_TO_MOUSE) == EXIT_SUCCESS)
		return EXIT_SUCCESS;
	if (issue_argument_KBC(argument) == EXIT_SUCCESS)
		return EXIT_SUCCESS;
	if (read_data_from_OUTBUF(KBC_value) == EXIT_SUCCESS)
		return EXIT_SUCCESS;
	if (KBC_value == ACK)
		return EXIT_SUCCESS;
}

int mouse_enable_stream_mode(){
	if (send_argument_mouse(MOUSE_STREAM_MODE) == EXIT_SUCCESS){
		/*memset(packet, 0, sizeof(packet));
		byte_number = 0;*/ // set all bytes bit 3 to 0 and byte_number (index of byte in packet of 3 bytes) to 0
		return EXIT_SUCCESS;
	}
	else
		return EXIT_FAILURE;
}

int mouse_disable_stream_mode(){
	if (send_argument_mouse(MOUSE_DISABLE_STREAM_MODE) == EXIT_SUCCESS){
		/*memset(packet, 0, sizeof(packet));
		byte_number = 0;*/ // set all bytes bit 3 to 0 and byte_number (index of byte in packet of 3 bytes) to 0
		return EXIT_SUCCESS;
	}
	else
		return EXIT_FAILURE;
}


int mouse_enable_sending_packets(){
	unsigned short verify;
	verify = send_argument_mouse(MOUSE_STREAM_MODE);
	if (verify == EXIT_SUCCESS){
		if (send_argument_mouse(MOUSE_SEND_PACKETS) == EXIT_SUCCESS){
			/*memset(packet, 0, sizeof(packet));
			byte_number = 0;*/ // set all bytes bit 3 to 0 and byte_number (index of byte in packet of 3 bytes) to 0
			return EXIT_SUCCESS;
		}
	}
	else
		return EXIT_FAILURE;
}

unsigned long mouse_int_handler(){
	unsigned long stat;
	if (read_data_from_OUTBUF(stat) == EXIT_SUCCESS){
		packet[byte_number] = stat;
		/*if (byte_number == 0 && (packet[byte_number] & BIT(3)) == 0)
			return;*/
		byte_number = (byte_number + 1) % 3	;
		return EXIT_SUCCESS;
	}
	else
		return EXIT_FAILURE;

}
