#include <minix/syslib.h>
#include <minix/drivers.h>

int timer_set_square(unsigned long timer, unsigned long freq) {

	return 1;
}


/* --- DÚVIDAS ----

// ESCLARECER DIFERENÇA ENTRE "int sys_outb" e "int sys_inb" !!!
// O QUE É TIMER_RB_SEL(n)         BIT((n)+1) ??


*/

int timer_subscribe_int(void ) {

	return 1;
}

int timer_unsubscribe_int() {

	return 1;
}

void timer_int_handler() {

}

int timer_get_conf(unsigned long timer, unsigned char *st) {
	
	return 1;
}

int timer_display_conf(unsigned char conf) {
	
	return 1;
}

int timer_test_square(unsigned long freq) {
	
	return 1;
}

int timer_test_int(unsigned long time) {
	
	return 1;
}

int timer_test_config(unsigned long timer) {
	
	return 1;
}
