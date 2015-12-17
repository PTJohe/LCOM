#include <minix/drivers.h>

#include "test4.h"

static void print_usage(char *argv[]);
static int proc_args(int argc, char *argv[]);
static unsigned long parse_ulong(char *str, int base);
static long parse_long(char *str, int base);

int main(int argc, char *argv[]) {
	/* Initialize service */
	sef_startup();

	/* Enable IO-sensitive operations for ourselves (used in lab3 to allow user level programs to executo I/O instructions)
	 sys_enable_iop(SELF);
	 */

	if (argc == 1) {
		print_usage(argv);
		return 0;
	} else {
		proc_args(argc, argv);
	}
	return 0;
}

static void print_usage(char *argv[]) {
	printf("Usage: one of the following:\n"
			"\t service run %s -args \"packet <cnt>\" \n"
			"\t service run %s -args \"async <timeout>\" \n"
			"\t service run %s -args \"config <>\" \n"
			"\t service run %s -args \"gesture <length> <tolerance>\" \n",
			argv[0], argv[0], argv[0], argv[0]);
}

static int proc_args(int argc, char *argv[]) {
	unsigned short cnt, idle_time, tolerance;
	short length;

	/* check the function to test: if the first characters match, accept it */
	if (strncmp(argv[1], "packet", strlen("packet")) == 0) {
		if (argc != 3) {
			printf(
					"test_packet: wrong no of arguments for test of test_packet \n");
			return EXIT_FAILURE;
		}
		cnt = parse_ulong(argv[2], 10);
		printf("test:: packet(%d)\n", (unsigned short) cnt);

		if (cnt == ULONG_MAX || cnt < 1)
			return EXIT_FAILURE;
		test_packet(cnt);
		return EXIT_SUCCESS;
	} else if (strncmp(argv[1], "async", strlen("async")) == 0) {
		if (argc != 3) {
			printf(
					"test_async: wrong no of arguments for test of test_async() \n");
			return EXIT_FAILURE;
		}
		idle_time = parse_ulong(argv[2], 10);
		printf("test:: async(%d)\n", (unsigned short) idle_time);
		if (idle_time == ULONG_MAX)
			return EXIT_FAILURE;
		test_async(idle_time);
		return EXIT_SUCCESS;
	} else if (strncmp(argv[1], "config", strlen("config")) == 0) {
		if (argc != 2) {
			printf(
					"test_config: wrong no of arguments for test of test_config() \n");
			return EXIT_FAILURE;
		}
		printf("test:: config()\n");
		test_config();
		return EXIT_SUCCESS;
	} /*else if (strncmp(argv[1], "gesture", strlen("gesture")) == 0) {
		if (argc != 4) {
			printf(
					"test_gesture: wrong no of arguments for test of test_gesture() \n");
			return EXIT_FAILURE;
		}
		length = parse_ulong(argv[2], 10);
		tolerance = parse_ulong(argv[3], 10);
		printf("test:: gesture(%d, %d)\n", (unsigned short) length,
				(unsigned short) tolerance);
		if (length == ULONG_MAX)
			return EXIT_FAILURE;
		if (tolerance == ULONG_MAX)
			return EXIT_FAILURE;
		test_gesture(length, tolerance);
		return EXIT_SUCCESS;
	}*/
}

static unsigned long parse_ulong(char *str, int base) {
	char *endptr;
	unsigned long val;

	val = strtoul(str, &endptr, base);

	if ((errno == ERANGE && val == ULONG_MAX) || (errno != 0 && val == 0)) {
		perror("strtol");
		return ULONG_MAX;
	}

	if (endptr == str) {
		printf("mouse_test: parse_ulong: no digits were found in %s \n", str);
		return ULONG_MAX;
	}

	/* Successful conversion */
	return val;
}

static long parse_long(char *str, int base) {
	char *endptr;
	unsigned long val;

	val = strtol(str, &endptr, base);

	if ((errno == ERANGE && (val == LONG_MAX || val == LONG_MIN))
			|| (errno != 0 && val == 0)) {
		perror("strtol");
		return LONG_MAX;
	}

	if (endptr == str) {
		printf("mouse_test: parse_long: no digits were found in %s \n", str);
		return LONG_MAX;
	}

	/* Successful conversion */
	return val;
}
