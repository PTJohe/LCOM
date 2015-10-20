#include <minix/drivers.h>

#include "test3.h"

static int proc_args(int argc, char *argv[]);
static unsigned long parse_ulong(char *str, int base);
static long parse_long(char *str, int base);
static void print_usage(char *argv[]);

int main(int argc, char **argv) {

	/* Initialize service */
	sef_startup();

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
			"\t service run %s -args \"scan <IH>\" \n"
			"\t service run %s -args \"leds <no elems> <leds array>\" \n"
			"\t service run %s -args \"timed_scan <timeout>\" \n", argv[0],
			argv[0], argv[0]);
}

static int proc_args(int argc, char *argv[]) {

	unsigned long ass, nElems, timeout;

	/* check the function to test: if the first characters match, accept it */
	if (strncmp(argv[1], "scan", strlen("scan")) == 0) {
		if (argc != 3) {
			printf(
					"kbd_test:: wrong no of arguments for test of kbd_test_scan() \n");
			return 1;
		}
		printf("kbd_test::kbd_test_scan()\n");
		if ((ass = parse_ulong(argv[2], 10)) == ULONG_MAX)
			return 1;
		kbd_test_scan(ass);
		return 0;
	} else if (strncmp(argv[1], "leds", strlen("leds")) == 0) {
		if (argc < 4) {
			printf(
					"kbd_test:: wrong no of arguments for test of kbd_test_leds() \n");
			return 1;
		}
		printf("kbd_test::kbd_test_leds() \n");
		if ((nElems = parse_ulong(argv[2], 10)) == ULONG_MAX)
			return 1;
		unsigned short toggle[nElems];
		int i = 0;
		for (i; i < nElems; i++) {
			if ((toggle[i] = parse_ulong(argv[i + 2], 10)) == ULONG_MAX)
				return 1;
		}
		kbd_test_leds(nElems, toggle);
		return 0;
	} else if (strncmp(argv[1], "timed_scan", strlen("timed_scan")) == 0) {
		if (argc != 3) {
			printf(
					"kbd_test:: wrong no of arguments for test of kbd_test_timed_scan() \n");
			return 1;
		}
		printf("kbd_test::kbd_test_timed_scan() \n");
		if ((timeout = parse_ulong(argv[2], 10)) == ULONG_MAX)
			return 1;
		kbd_test_timed_scan(timeout);
		return 0;
	} else {
		printf("kbd_test: non valid function \"%s\" to test\n", argv[1]);
		return 1;
	}
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
		printf("kbd_test: parse_ulong: no digits were found in %s \n", str);
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
		printf("kbd_test: parse_long: no digits were found in %s \n", str);
		return LONG_MAX;
	}

	/* Successful conversion */
	return val;
}
