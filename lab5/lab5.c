#include <minix/drivers.h>

#include "test5.h"

static void print_usage(char *argv[]);
static int proc_args(int argc, char *argv[]);
static unsigned long parse_ulong(char *str, int base);
static long parse_long(char *str, int base);

int main(int argc, char *argv[]) {
	/* Initialize service (synchronize with the RS) */
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
			"\t service run %s -args \"init <mode> <delay>\" \n"
			"\t service run %s -args \"square <x> <y> <size> <color>\" \n"
			"\t service run %s -args \"line <xi> <yi> <xf> <yf> <color>\" \n"
			"\t service run %s -args \"xpm <xi> <yi> <xpm_array>\" \n"
			"\t service run %s -args \"move <xi> <yi> <xpm_array> <hor> <delta> <time>\" \n"
			"\t service run %s -args \"controller <>\" \n",
			argv[0], argv[0], argv[0], argv[0], argv[0], argv[0]);
}

static int proc_args(int argc, char *argv[]) {
	unsigned short mode, delay, x, y, size, xi, yi, xf, yf, hor, delta, time;
	unsigned long color;
	//unsigned char* xpm[];

	/* check the function to test: if the first characters match, accept it */
	if (strncmp(argv[1], "init", strlen("init")) == 0) {
		if (argc != 4) {
			printf(
					"test_init: wrong no of arguments for test_init \n");
			return EXIT_FAILURE;
		}
		mode = parse_ulong(argv[2], 10);
		delay = parse_ulong(argv[3], 10);
		printf("test:: init(%d, %d)\n", (unsigned short) mode, (unsigned short) delay);
		if (mode == ULONG_MAX || delay == ULONG_MAX)
			return EXIT_FAILURE;
		test_init(mode, delay);
		return EXIT_SUCCESS;
	} else if (strncmp(argv[1], "square", strlen("square")) == 0) {
		if (argc != 6) {
			printf(
					"test_square: wrong no of arguments for test_square() \n");
			return EXIT_FAILURE;
		}
		x = parse_ulong(argv[2], 10);
		y = parse_ulong(argv[3], 10);
		size = parse_ulong(argv[4], 10);
		color = parse_ulong(argv[5], 10);
		printf("test:: square(%d, %d, %d, %d)\n", (unsigned short) x, (unsigned short) y, (unsigned short) size, (unsigned long) color);
		if (x == ULONG_MAX || y == ULONG_MAX || size == ULONG_MAX || color == ULONG_MAX)
			return EXIT_FAILURE;
		test_square(x, y, size, color);
		return EXIT_SUCCESS;
	} else if (strncmp(argv[1], "line", strlen("line")) == 0) {
		if (argc != 7) {
			printf(
					"test_line: wrong no of arguments for test_line() \n");
			return EXIT_FAILURE;
		}
		xi = parse_ulong(argv[2], 10);
		yi = parse_ulong(argv[3], 10);
		xf = parse_ulong(argv[4], 10);
		yf = parse_ulong(argv[5], 10);
		color = parse_ulong(argv[6], 10);
		if (xi == ULONG_MAX || yi == ULONG_MAX || xf == ULONG_MAX || yf == ULONG_MAX || color == ULONG_MAX)
			return EXIT_FAILURE;
		printf("test:: line(%d, %d, %d, %d, %d)\n", (unsigned short) xi, (unsigned short) yi, (unsigned short) xf, (unsigned short) yf, (unsigned long) color);
		test_line(xi, yi, xf, yf, color);
		return EXIT_SUCCESS;
	} /*else if (strncmp(argv[1], "xpm", strlen("xpm")) == 0) {
		if (argc != 5) {
			printf(
					"test_xpm: wrong no of arguments for test_xpm() \n");
			return EXIT_FAILURE;
		}
		xi = parse_ulong(argv[2], 10);
		yi = parse_ulong(argv[3], 10);


		printf("test:: xpm(%d, %d)\n", (unsigned short) length,
				(unsigned short) tolerance);
		if (length == ULONG_MAX)
			return EXIT_FAILURE;
		if (tolerance == ULONG_MAX)
			return EXIT_FAILURE;
		test_gesture(length, tolerance);
		return EXIT_SUCCESS;
	} else if (strncmp(argv[1], "move", strlen("move")) == 0) {
		if (argc != 8) {
			printf("test_move: wrong no of arguments for test_move() \n");
			return EXIT_FAILURE;
		}
		xi = parse_ulong(argv[2], 10);
		yi = parse_ulong(argv[3], 10);


		printf("test:: move(%d, %d)\n", (unsigned short) length,
				(unsigned short) tolerance);
		if (length == ULONG_MAX)
			return EXIT_FAILURE;
		if (tolerance == ULONG_MAX)
			return EXIT_FAILURE;
		test_gesture(length, tolerance);
		return EXIT_SUCCESS;
	}*/else if (strncmp(argv[1], "controller", strlen("controller")) == 0) {
		if (argc != 2) {
			printf("test_controller: wrong no of arguments for test_controller() \n");
			return EXIT_FAILURE;
		}
		printf("test:: controller()\n");
		test_controller();
		return EXIT_SUCCESS;
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
		printf("video_graphics: parse_ulong: no digits were found in %s \n", str);
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
		printf("video_graphics: parse_long: no digits were found in %s \n", str);
		return LONG_MAX;
	}

	/* Successful conversion */
	return val;
}
