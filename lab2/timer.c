#include <minix/syslib.h>
#include <minix/drivers.h>
#include <stdio.h>
#include <stdlib.h>

int timer_set_square(unsigned long timer, unsigned long freq) {

	return 1;
}


/* --- DÚVIDAS ----
 *
// ESCLARECER DIFERENÇA ENTRE "int sys_outb" e "int sys_inb" !!!
// O QUE É TIMER_RB_SEL(n)         BIT((n)+1) ??
// VERIFICAR SE MAKEFILE ESTÁ CORRETO !!

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
	unsigned long cmd = TIMER_RB_CMD | TIMER_SEL(timer) | TIMER_RB_COUNT;
	if (timer > 2 || timer < 0){
		return EXIT_FAILURE;
	}
	else {
		sys_outb(TIMER_CTRL, cmd);
		sys_inb(TIMER_0 + timer, st);
		return 0;
	}
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
	unsigned char valor_st, conf;
	timer_get_conf(timer, &valor_st);
	timer_display_conf(conf)
	return 1;
}
