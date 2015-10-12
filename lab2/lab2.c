#include <minix/syslib.h>
#include <minix/drivers.h>
#include <stdio.h>
#include "timer.h"
#include "i8254.h"
#include <limits.h>
#include <errno.h>

static int proc_args(int argc, char *argv[]);
static unsigned long parse_ulong(char *str, int base);
static long parse_long(char *str, int base);
static void print_usage(char *argv[]);

int main(int argc, char **argv) {

  /* Initialize service */
  sef_startup();

  if ( argc == 1 ) {
      print_usage(argv);
      return 0;
  } else {
      proc_args(argc, argv);
  }
  return 0;

}

static void print_usage(char *argv[]) {
  printf("Usage: one of the following:\n"
		  "\t service run %s -args \"config <timer no>\" \n"
		  "\t service run %s -args \"square <freq no>\" \n"
		  "\t service run %s -args \"int <time no>\" \n" ,
		  argv[0], argv[0], argv[0]);
}

static int proc_args(int argc, char *argv[]) {

  unsigned long timer, frequency, time;

  /* check the function to test: if the first characters match, accept it */

  if (strncmp(argv[1], "config", strlen("config")) == 0) {
	  if( argc != 3 ) {
  		  printf("timer_test: wrong no of arguments for test of timer_test_config \n");
  		  return 1;
  	  }
	  if( (timer = parse_ulong(argv[2], 10)) == ULONG_MAX )
		  return 1;
	  if (timer < 0 || timer > 2 )
		  return 1;
	  printf("timer_test:: timer_test_config(%ul)\n", timer);
	  timer_test_config(timer);
	  return 0;

    } else if (strncmp(argv[1], "square", strlen("square")) == 0) {
  	  if( argc != 3 ) {
  		  printf("timer_test: wrong no of arguments for test of timer_test_square \n");
  		  return 1;
  	  }
  	  if( (frequency = parse_ulong(argv[2], 10)) == ULONG_MAX )
  		  return 1;
  	  printf("timer_test:: timer_test_square(%ul)\n", frequency);
  	  return timer_test_square(frequency);

    } else if (strncmp(argv[1], "int", strlen("int")) == 0) {
  	  if( argc != 3 ) {
  		  printf("timer_test: wrong no of arguments for test of timer_test_int \n");
  		  return 1;
  	  }
  	  if( (time = parse_ulong(argv[2], 10)) == ULONG_MAX )
  		  return 1;
  	  printf("timer_test:: timer_test_int(%ul)\n", time);
  	  timer_test_int(time);
  	  return 1;

    } else {
    	printf("timer_test: non valid function \"%s\" to test\n", argv[1]);
    	return 1;
    }
}



static unsigned long parse_ulong(char *str, int base) {
  char *endptr;
  unsigned long val;

  val = strtoul(str, &endptr, base);

  if ((errno == ERANGE && val == ULONG_MAX )
	  || (errno != 0 && val == 0)) {
	  perror("strtol");
	  return ULONG_MAX;
  }

  if (endptr == str) {
	  printf("timer_test: parse_ulong: no digits were found in %s \n", str);
	  return ULONG_MAX;
  }

  /* Successful conversion */
  return val;

}
