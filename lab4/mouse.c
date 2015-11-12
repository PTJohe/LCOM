#include "mouse.h"
#include "i8042.h"
#include "keyboard.h"

int mouse_subscribe_int(){
	int bitmask = BIT(hook_id_mouse);
	if (sys_irqsetpolicy(MOUSE_IRQ, IRQ_REENABLE | IRQ_EXCLUSIVE, &hook_id_mouse) == OK
			&& sys_irqenable(&hook_id_mouse) == OK){
		memset(packet, 0, sizeof(packet));
		byte_number = 0; // set all bytes bit 3 to 0 and byte_number (index of byte in packet of 3 bytes) to 0
		return bitmask;
	}
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
	if (KBC_issue_command_mouse(WRITE_BYTE_TO_MOUSE) == EXIT_FAILURE)
		return EXIT_FAILURE;
	if (issue_argument_KBC(argument) == EXIT_FAILURE)
		return EXIT_FAILURE;
	if (read_data_OUTBUF_from_KBC(&KBC_value) == EXIT_FAILURE)
		return EXIT_FAILURE;
	if (KBC_value == ACK)
		return EXIT_SUCCESS;
	else
		return EXIT_FAILURE;
}

unsigned short read_data_OUTBUF_from_MOUSE(unsigned long* data){
	unsigned long status_register;
		while (1) {
				if (sys_inb(KBC_STAT_REG, &status_register) != OK)
					return EXIT_FAILURE;
				if (status_register & KBC_OBF) {
					if (sys_inb(KBC_OUT_BUF, data) == OK) {
						if ((status_register & (PAR_ERR | TO_ERR)) == 0 && (status_register & AUX) == 1) // AUX = 1 indicates that data in OUT_BUF comes from MOUSE
							return EXIT_SUCCESS;
					} else
						return EXIT_FAILURE;
				}
				tickdelay(micros_to_ticks(DELAY_US));
			}

}

int mouse_enable_stream_mode(){
	if (send_argument_mouse(MOUSE_STREAM_MODE) == EXIT_SUCCESS){
		return EXIT_SUCCESS;
	}
	else
		return EXIT_FAILURE;
}

int mouse_disable_stream_mode(){
	if (send_argument_mouse(MOUSE_DISABLE_STREAM_MODE) == EXIT_SUCCESS){
		memset(packet, 0, sizeof(packet));
		byte_number = 0; // set all bytes bit 3 to 0 and byte_number (index of byte in packet of 3 bytes) to 0
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
			memset(packet, 0, sizeof(packet));
			byte_number = 0; // set all bytes bit 3 to 0 and byte_number (index of byte in packet of 3 bytes) to 0
			return EXIT_SUCCESS;
		}
	}
		else
			return EXIT_FAILURE;
	}

unsigned long mouse_int_handler(){
	unsigned long stat;
	if (read_data_OUTBUF_from_KBC(&stat) != EXIT_FAILURE){
			packet[byte_number] = stat;

			if (byte_number == 0 && (packet[0] & BIT(3) == 0))
				return EXIT_FAILURE;
			byte_number = (byte_number + 1) % 3	;
	}
	return EXIT_SUCCESS;
}

/*unsigned long mouse_int_handler(){
	unsigned long stat;
	int synced = 0;
	int bytes_zero = 0;
	if (read_data_OUTBUF_from_KBC(&stat) != EXIT_FAILURE){
		if (!synced){
			if (stat & BIT(3) == 0)
				bytes_zero++;
			else
				bytes_zero = 0;
			if (bytes_zero == 2){
				synced = 1;
				bytes_zero = 0;
			}
		}
		else{
			packet[bytes_zero] = stat;
			bytes_zero++;
			if (bytes_zero == 2){
				bytes_zero = 0;
				return EXIT_SUCCESS;
			}
		}

	}
	//return EXIT_SUCCESS;
}*/
