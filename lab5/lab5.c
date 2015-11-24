#include <minix/drivers.h>

#include "test5.h"
#include "pixmap.h"

static void print_usage(char *argv[]);
static int proc_args(int argc, char *argv[]);
static long parse_long(char *str, int base);
static unsigned long parse_ulong(char *str, int base);
static short parse_short(char *str, int base);
static unsigned short parse_ushort(char *str, int base);

int main(int argc, char *argv[]) {
	/* Initialize service (synchronize with the RS) */
	sef_startup();

	if (argc == 1) {
		print_usage(argv);
		return EXIT_SUCCESS;
	} else
		return proc_args(argc, argv);
}

static void print_usage(char *argv[]) {
	printf(
			"Usage: one of the following:\n"
					"\t service run %s -args \"init <mode> <delay>\" \n"
					"\t service run %s -args \"square <x> <y> <size> <color>\" \n"
					"\t service run %s -args \"line <xi> <yi> <xf> <yf> <color>\" \n"
					"\t service run %s -args \"xpm <xi> <yi> <xpm_array>\" \n"
					"\t service run %s -args \"move <xi> <yi> <xpm_array> <hor> <delta> <time>\" \n"
					"\t service run %s -args \"controller <>\" \n", argv[0],
			argv[0], argv[0], argv[0], argv[0], argv[0]);
}

static int proc_args(int argc, char *argv[]) {
	unsigned short mode, delay, x, y, size, xi, yi, xf, yf, xpm, hor, time;
	unsigned long color;
	short delta;

	/* check the function to test: if the first characters match, accept it */
	if (strncmp(argv[1], "init", strlen("init")) == 0) {
		if (argc != 4) {
			printf("lab5::test_init: wrong no of arguments for test_init()\n");
			return EXIT_FAILURE;
		}
		mode = parse_ushort(argv[2], 16);
		delay = parse_ushort(argv[3], 10);

		printf("lab5::test_init(0x%X, %d)\n", mode, delay);
		if (mode == USHRT_MAX || delay == USHRT_MAX) {
			printf("ERROR: Invalid arguments!\n");
			return EXIT_FAILURE;
		}
		test_init(mode, delay);
		return EXIT_SUCCESS;

	} else if (strncmp(argv[1], "square", strlen("square")) == 0) {
		if (argc != 6) {
			printf(
					"lab5::test_square: wrong no of arguments for test_square()\n");
			return EXIT_FAILURE;
		}
		x = parse_ushort(argv[2], 10);
		y = parse_ushort(argv[3], 10);
		size = parse_ushort(argv[4], 10);
		color = parse_ulong(argv[5], 16);

		printf("lab5::test_square(%d, %d, %d, %d)\n", x, y, size, color);
		if (x == USHRT_MAX || y == USHRT_MAX || size == USHRT_MAX
				|| color == USHRT_MAX) {
			printf("ERROR: Invalid arguments!\n");
			return EXIT_FAILURE;
		}
		return test_square(x, y, size, color);

	} else if (strncmp(argv[1], "line", strlen("line")) == 0) {
		if (argc != 7) {
			printf("lab5::test_line: wrong no of arguments for test_line()\n");
			return EXIT_FAILURE;
		}
		xi = parse_ushort(argv[2], 10);
		yi = parse_ushort(argv[3], 10);
		xf = parse_ushort(argv[4], 10);
		yf = parse_ushort(argv[5], 10);
		color = parse_ulong(argv[6], 16);

		printf("lab5::test_line(%d, %d, %d, %d, %d)\n", xi, yi, xf, yf, color);
		if (xi == USHRT_MAX || yi == USHRT_MAX || xf == USHRT_MAX
				|| yf == USHRT_MAX || color == USHRT_MAX) {
			printf("ERROR: Invalid arguments!\n");
			return EXIT_FAILURE;
		}
		return test_line(xi, yi, xf, yf, color);

	} else if (strncmp(argv[1], "xpm", strlen("xpm")) == 0) {
		if (argc != 5) {
			printf("lab5::test_xpm: wrong no of arguments for test_xpm()\n");
			return EXIT_FAILURE;
		}
		xi = parse_ushort(argv[2], 10);
		yi = parse_ushort(argv[3], 10);
		xpm = parse_ushort(argv[4], 10);

		printf("lab5::test_xpm(%d, %d, %d)\n", xi, yi, xpm);
		if (xi == USHRT_MAX || yi == USHRT_MAX || xpm == USHRT_MAX) {
			printf("ERROR: Invalid arguments!\n");
			return EXIT_FAILURE;
		}
		if (getPixmap(xpm) == NULL) {
			printf("ERROR: Could not find xpm!\n");
			return EXIT_FAILURE;
		}
		return test_xpm(xi, yi, getPixmap(xpm));
	} else if (strncmp(argv[1], "move", strlen("move")) == 0) {
		if (argc != 8) {
			printf("lab5::test_move: wrong no of arguments for test_move()\n");
			return EXIT_FAILURE;
		}
		xi = parse_ushort(argv[2], 10);
		yi = parse_ushort(argv[3], 10);
		xpm = parse_ushort(argv[4], 10);
		hor = parse_ushort(argv[5], 10);
		delta = parse_short(argv[6], 10);
		time = parse_ushort(argv[7], 10);

		printf("lab5::test_move(%d, %d, %d, %d, %d)\n", xi, yi, xpm, hor, delta,
				time);
		if (xi == USHRT_MAX || yi == USHRT_MAX || xpm == USHRT_MAX
				|| hor == USHRT_MAX || delta == SHRT_MAX || time == USHRT_MAX) {
			printf("ERROR: Invalid arguments!\n");
			return EXIT_FAILURE;
		}
		if (getPixmap(xpm) == NULL) {
			printf("ERROR: Could not find xpm!\n");
			return EXIT_FAILURE;
		}
		return test_move(xi, yi, getPixmap(xpm), hor, delta, time);
	} else if (strncmp(argv[1], "controller", strlen("controller")) == 0) {
		if (argc != 2) {
			printf(
					"lab5::test_controller: wrong no of arguments for test_controller()\n");
			return EXIT_FAILURE;
		}
		printf("lab5::test_controller()\n");
		return test_controller();
	} else{
		printf("ERROR: Inexistent function!\n\n");
		print_usage(argv);
		return EXIT_FAILURE;
	}
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
		printf("lab5::parse_long: no digits were found in %s \n", str);
		return LONG_MAX;
	}

	/* Successful conversion */
	return val;
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
		printf("lab5::parse_ulong: no digits were found in %s \n", str);
		return ULONG_MAX;
	}

	/* Successful conversion */
	return val;
}

static short parse_short(char *str, int base) {
	char *endptr;
	unsigned long val;

	val = strtoul(str, &endptr, base);

	if ((errno == ERANGE && val == USHRT_MAX) || (errno != 0 && val == 0)) {
		perror("strtol");
		return SHRT_MAX;
	}

	if (endptr == str) {
		printf("lab5::parse_short: no digits were found in %s \n", str);
		return SHRT_MAX;
	}

	/* Successful conversion */
	return val;
}

static unsigned short parse_ushort(char *str, int base) {
	char *endptr;
	unsigned long val;

	val = strtoul(str, &endptr, base);

	if ((errno == ERANGE && val == USHRT_MAX) || (errno != 0 && val == 0)) {
		perror("strtol");
		return USHRT_MAX;
	}

	if (endptr == str) {
		printf("lab5::parse_ushort: no digits were found in %s \n", str);
		return USHRT_MAX;
	}

	/* Successful conversion */
	return val;
}
