#include "test4.h"

#include "i8042.h"
#include "mouse.h"
#include "keyboard.h"
#include "timer.h"

unsigned long packet_int_handler() {
	unsigned short leftmousebutton, rightmousebutton, middlemousebutton, x_sign, y_sign, x_overflow, y_overflow;
	if (mouse_int_handler() == EXIT_FAILURE)
		return EXIT_FAILURE;
	if (packet[0] & LEFT_BUTTON)
		leftmousebutton = 1;
	else
		leftmousebutton = 0;
	if (packet[0] & RIGHT_BUTTON)
		rightmousebutton = 1;
	else
		rightmousebutton = 0;
	if (packet[0] & MIDDLE_BUTTON)
		middlemousebutton = 1;
	else
		middlemousebutton = 0;
	if (packet[0] & X_SIGN)
		x_sign = 1;
	else
		x_sign = 0;
	if ( packet[0] & X_OVERFLOW )
		x_overflow = 1;
	else
		x_overflow = 0;
	//if (x_overflow) {
		if (x_sign)
			packet[1] |= (-1 << 8);
		/*else
			packet[1] = (-1 << 8) + 1;*/
	/*} else {
		if ((packet[0] & X_SIGN) == 1)
			packet[1] = (-1 << 8) | packet[1];
		else if ((packet[0] & X_SIGN) == 0)
			packet[1] = packet[1];
	}*/
	if (packet[0] & Y_SIGN)
			y_sign = 1;
		else
			y_sign = 0;
		if ( packet[0] & Y_OVERFLOW )
			y_overflow = 1;
		else
			y_overflow = 0;
	//if (y_overflow) {
		if (y_sign)
			packet[2] |= (-1 << 8);
		/*else
			packet[2] = (-1 << 8) + 1;*/
	/*} else {
		if ((packet[0] & Y_SIGN) == 1)
			packet[2] = (-1 << 8) | packet[2];
		else if ((packet[0] & Y_SIGN) == 0)
			packet[2] = packet[2];
	}*/
	printf("B1 = 0x%02X", packet[0]);
	printf(" B2 = 0x%02X", packet[1]);
	printf(" B3 = 0x%02X", packet[2]);
	printf(" LB: %d", leftmousebutton);
	printf(" RB: %d", rightmousebutton);
	printf(" MB: %d", middlemousebutton);
	printf(" XOV: %d", x_overflow);
	printf(" YOV: %d", y_overflow);
	printf(" X: %d", packet[1]);
	printf(" Y: %d \n", packet[2]);
	return EXIT_SUCCESS;

}

int test_packet(unsigned short cnt) {
	int irq_set = mouse_subscribe_int();
		if (irq_set == EXIT_FAILURE)
			return EXIT_FAILURE;
		if (KBC_issue_command_mouse(ENABLE_MOUSE) == EXIT_FAILURE)
			return EXIT_FAILURE;
		/*int stream_mode = mouse_enable_stream_mode();
		if (stream_mode == EXIT_FAILURE)
			return EXIT_FAILURE;*/
		int send_packets = mouse_enable_sending_packets();
		if (send_packets == EXIT_FAILURE)
			return EXIT_FAILURE;
		int r;
		int aux = 0;
		int ipc_status;
		message msg;
		while ((cnt * 3) > aux) { /* You may want to use a different condition */
			/* Get a request message. */
			if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) {
				printf("driver_receive failed with: %d", r);
				continue;
			}
			if (is_ipc_notify(ipc_status)) { /* received notification */
				switch (_ENDPOINT_P(msg.m_source)) {
				case HARDWARE: /* hardware interrupt notification */
					if (msg.NOTIFY_ARG & irq_set) {
						aux++;
						if (packet_int_handler() == EXIT_FAILURE)
							return EXIT_FAILURE;


					}
					break;
				default:
					break; /* no other notifications expected: do nothing */
				}
			} else { /* received a standard message, not a notification */
				/* no standard messages expected: do nothing */
			}
		}

		int disable_stream_mode = mouse_disable_stream_mode();
		if (disable_stream_mode == EXIT_FAILURE)
			return EXIT_FAILURE;
		if (mouse_unsubscribe_int() == EXIT_FAILURE) {
			printf("ERROR: mouse_unsubscribe_int failed!");
			return EXIT_FAILURE;
		}
		printf("Test_packet concluido.\n");
		return EXIT_SUCCESS;
	}



int test_async(unsigned short idle_time) {
	int mouse_irq = mouse_subscribe_int();
	int timer_irq = timer_subscribe_int();
	if (KBC_issue_command_mouse(ENABLE_MOUSE) == EXIT_FAILURE)
			return EXIT_FAILURE;
		int stream_mode = mouse_enable_stream_mode();
		if (stream_mode == EXIT_FAILURE)
			return EXIT_FAILURE;
		int send_packets = mouse_enable_sending_packets();
		if (send_packets == EXIT_FAILURE)
			return EXIT_FAILURE;
	int r;
	int ipc_status;
	unsigned int time = 0;
	message msg;

	while (time < (60 * idle_time) ) {
		/* Get a request message. */
		if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) {
			printf("driver_receive failed with: %d", r);
			continue;
		}
		if (is_ipc_notify(ipc_status)) { /* received notification */
			switch (_ENDPOINT_P(msg.m_source)) {
			case HARDWARE: /* hardware interrupt notification */
				if (msg.NOTIFY_ARG & mouse_irq) {
					time = 0;
					if (packet_int_handler() == EXIT_FAILURE)
						return EXIT_FAILURE;
				}
				if (msg.NOTIFY_ARG & timer_irq)
					time++;
				break; /* no other notifications expected: do nothing */
			default:
				break;
			}
		} else { /* received a standard message, not a notification */
			/* no standard messages expected: do nothing */
		}
	}
	int disable_stream_mode = mouse_disable_stream_mode();
	if (disable_stream_mode == EXIT_FAILURE)
		return EXIT_FAILURE;

	if (mouse_unsubscribe_int() != EXIT_SUCCESS) {
		printf("ERROR: failed to unsubscribe kbd interrupts.\n");
		return 1;
	}
	if (timer_unsubscribe_int() != EXIT_SUCCESS) {
		printf("ERROR: failed to unsubscribe timer interrupts.\n");
		return 1;
	}
	printf("test_async concluido devido a timeout.\n");
	return EXIT_SUCCESS;
}

int test_config(void) {
	int mouse_irq = mouse_subscribe_int();
	if (mouse_irq == EXIT_FAILURE)
		return EXIT_FAILURE;
	if (KBC_issue_command_mouse(ENABLE_MOUSE) == EXIT_FAILURE)
		return EXIT_FAILURE;
	int disable_stream_mode = mouse_disable_stream_mode(); // bit 5 of mouse status will always be 0 (disabled sending data packets)
	if (disable_stream_mode == EXIT_FAILURE)
		return EXIT_FAILURE;
	unsigned short status_request = send_argument_mouse(MOUSE_STATUS_REQUEST);
	if (status_request == EXIT_FAILURE)
		return EXIT_FAILURE;
	/*for (i = 0; i < 3; i++) { // 3 is the size of the packet (3 bytes) that contains the mouse "status" - STATUS REQUEST
		if (read_data_OUTBUF_from_MOUSE(&packet[i]) == EXIT_FAILURE)
			return EXIT_FAILURE;
	}*/
	if (mouse_int_handler() == EXIT_FAILURE)
			return EXIT_FAILURE;

	if (packet[0] & RIGHT_STATUS_BUTTON)
		printf("Right Button: pressed\n");
	else
		printf("Right Button: released\n");
	if (packet[0] & MIDDLE_STATUS_BUTTON)
		printf("Middle Button: pressed\n");
	else
		printf("Middle Button: released\n");
	if (packet[0] & LEFT_STATUS_BUTTON)
		printf("Left Button: pressed\n");
	else
		printf("Left Button: released\n");
	if (packet[0] & SCALLING)
		printf("Scalling: 2:1\n");
	else
		printf("Scalling: 1:1\n");
	if (packet[0] & SEND_PACKETS_ENABLED)
		printf("Sending data packets: enabled\n"); // never occurs, because stream mode was disabled previously
	else
		printf("Sending data packets: disabled\n");
	if (packet[0] & REMOTE)
		printf("Mode: remote (polled) mode\n");
	else
		printf("Mode: stream mode\n");
	switch (packet[1]) {
	case 0:
		printf("Resolution: 1 count per mm\n");
		break;
	case 1:
		printf("Resolution: 2 counts per mm\n");
		break;
	case 2:
		printf("Resolution: 4 counts per mm\n");
		break;
	case 3:
		printf("Resolution: 8 counts per mm\n");
		break;
	}
	printf("Sample rate: %d samples per second\n", packet[2]); // possible values for sample rate: 10, 20, 40, 60, 80, 100, 200
	if (mouse_unsubscribe_int() == EXIT_FAILURE)
		return EXIT_FAILURE;
	printf("Test config concluido!");
	return EXIT_SUCCESS;
}

/*unsigned long check_hor_line(ev_type_t event, short length, short tolerance, int deltaX, int deltaY) {
	static state_t st = INIT; // initial state; keep state
	short hor_stat, vert_stat;
		switch (st) {
		case INIT:
			if( event == RDOW )
				st = DRAW;
			break;
		case DRAW:
			switch (event){
			case RUP:
				st = INIT;
				hor_stat = 0;
				vert_stat = 0;
				break;
			case MOVE:
				st = DRAW;
				vert_stat = vert_stat + deltaY;
				if ( ((tolerance * 0.5) >= abs(vert_stat)) && ((deltaX ^ length) > 0 )){
					hor_stat = hor_stat + deltaX;
					if ( (length ^ hor_stat) > 0  && ( abs(length) < abs(hor_stat)))
						return EXIT_SUCCESS;
				}
				else{
					printf("Vertical line reseted, because tolerance was exceeded");
					hor_stat = 0;
					vert_stat = 0;
				}
				break;
			default:
				break;
			}
			break;
		}
		return EXIT_FAILURE;
}*/

/*unsigned long gesture_int_handler(short length, unsigned short tolerance){
	unsigned short leftmousebutton, rightmousebutton, middlemousebutton, x_sign, y_sign, x_overflow, y_overflow;
		if (mouse_int_handler() == EXIT_FAILURE)
			return EXIT_FAILURE;
		leftmousebutton = packet[0] & LEFT_BUTTON;
		rightmousebutton = packet[0] & RIGHT_BUTTON;
		middlemousebutton = packet[0] & MIDDLE_BUTTON;
		x_sign = packet[0] & X_SIGN;
		x_overflow = packet[0] & X_OVERFLOW;
		if (x_overflow) {
			if (x_sign)
				packet[1] = (1 << 8) - 1;
			else
				packet[1] = (-1 << 8) + 1;
		} else {
			if ((packet[0] & X_SIGN) == 1)
				packet[1] = (-1 << 8) | packet[1];
			else if ((packet[0] & X_SIGN) == 0)
				packet[1] = packet[1];
		}
		y_sign = packet[0] & Y_SIGN;
		y_overflow = packet[0] & Y_OVERFLOW;
		if (y_overflow) {
			if (y_sign)
				packet[2] = (1 << 8) - 1;
			else
				packet[2] = (-1 << 8) + 1;
		} else {
			if ((packet[0] & Y_SIGN) == 1)
				packet[2] = (-1 << 8) | packet[2];
			else if ((packet[0] & Y_SIGN) == 0)
				packet[2] = packet[2];
		}
		if (leftmousebutton){
			event = RDOW;
			if (check_hor_line(event, length, tolerance, packet[1], packet[2]) != EXIT_FAILURE)
				return EXIT_SUCCESS;
		}
		else{
			event = RUP;
			if (check_hor_line(event, length, tolerance, packet[1], packet[2]) != EXIT_FAILURE)
				return EXIT_SUCCESS;
		}
		if (packet[1] != 0 || packet[2] != 0){
			event = MOVE;
			if (check_hor_line(event, length, tolerance, packet[1], packet[2]) != EXIT_FAILURE)
				return EXIT_SUCCESS;
		}
		return EXIT_FAILURE;
}*/

/*int test_gesture(short length, unsigned short tolerance) {
	int irq_set = mouse_subscribe_int();
		if (irq_set == EXIT_FAILURE)
			return EXIT_FAILURE;
		if (KBC_issue_command_mouse(ENABLE_MOUSE) == EXIT_FAILURE)
			return EXIT_FAILURE;
		int stream_mode = mouse_enable_stream_mode();
		if (stream_mode == EXIT_FAILURE)
			return EXIT_FAILURE;
		int send_packets = mouse_enable_sending_packets();
		if (send_packets == EXIT_FAILURE)
			return EXIT_FAILURE;
		int r;
		int n = 0;
		int ipc_status;
		message msg;
		while (1) { // You may want to use a different condition
			// Get a request message.
			if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) {
				printf("driver_receive failed with: %d", r);
				continue;
			}
			if (is_ipc_notify(ipc_status)) { // received notification
				switch (_ENDPOINT_P(msg.m_source)) {
				case HARDWARE: // hardware interrupt notification
					if (msg.NOTIFY_ARG & irq_set) {
						if (gesture_int_handler(length, tolerance) == EXIT_FAILURE)
							return EXIT_FAILURE;
						else
							break;
					}
					break;
				default:
					break; // no other notifications expected: do nothing
				}
			} else { // received a standard message, not a notification
				// no standard messages expected: do nothing
			}
		}

		int disable_stream_mode = mouse_disable_stream_mode();
		if (disable_stream_mode == EXIT_FAILURE)
			return EXIT_FAILURE;
		if (mouse_unsubscribe_int() == EXIT_FAILURE) {
			printf("ERROR: mouse_unsubscribe_int failed!");
			return EXIT_FAILURE;
		}
		printf("Test_gesture concluido.\n");
		return EXIT_SUCCESS;
}*/
