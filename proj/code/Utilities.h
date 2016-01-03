#ifndef __CONSTANTS_H
#define __CONSTANTS_H

#include <stdio.h>

#define BIT(n) (0x01 << n)

//==========================================================================================
/*
 * LOGGER MACROS
 */

extern FILE* logfd;
#define LOG_PATH "home/lcom/lcom1516-t2g15/proj/log.txt"
#define LOG(tag, msg) fprintf(logfd, "%s: %s\n", tag, msg)
#define LOG_VAR(tag, var) fprintf(logfd, "%s: %d\n", tag, var)

//==========================================================================================
/*
 * TIMER MACROS
 */

#define TIMER0_IRQ 0    /**< @brief Timer 0 IRQ line */
#define TIMER_FREQ  1193182    /**< @brief Clock frequency for timer in PC and AT */
#define TIMER_DEFAULT_FREQ 60.0 /**< @brief Default interrupt frequency of timer = 60 Hz */

//I/O port addresses
#define TIMER_0			0x40 /**< @brief Timer 0 count register */
#define TIMER_1			0x41 /**< @brief Timer 1 count register */
#define TIMER_2			0x42 /**< @brief Timer 2 count register */
#define TIMER_CTRL		0x43 /**< @brief Control register */
#define SPEAKER_CTRL		0x61 /**< @brief Register for speaker control  */

//Timer selection: bits 7 and 6
#define TIMER_SEL0		0x00     /**< @brief Control Word for Timer 0 */
#define TIMER_SEL1		BIT(6)   /**< @brief Control Word for Timer 1 */
#define TIMER_SEL2		BIT(7)   /**< @brief Control Word for Timer 2 */
#define TIMER_RB_CMD		(BIT(7)|BIT(6))  /**< @brief Read Back Command */

//Register selection: bits 5 and 4
#define TIMER_LSB		BIT(4)  /**< @brief Initialize Counter LSB only */
#define TIMER_MSB		BIT(5)  /**< @brief Initialize Counter MSB only */
#define TIMER_LSB_MSB		(TIMER_LSB | TIMER_MSB) /**< @brief Initialize LSB first and MSB afterwards */

//Operating mode: bits 3, 2 and 1
#define TIMER_SQR_WAVE		(BIT(2)|BIT(1)) /**< @brief Mode 3: square wave generator */
#define TIMER_RATE_GEN		BIT(2)          /**< @brief Mode 2: rate generator */

//Counting mode: bit 0
#define TIMER_BCD			0x01   /**< @brief Count in BCD */
#define TIMER_BIN			0x00   /**< @brief Count in binary */

//Read-back command format
#define TIMER_RB_COUNT_         BIT(5)
#define TIMER_RB_STATUS_        BIT(4)
#define TIMER_RB_SEL(n)         BIT((n)+1)

//==========================================================================================
/*
 * KEYBOARD MACROS
 */

#define DELAY_US    20000

#define KBC_IRQ 1 /**< @brief Keyboard IRQ line */
#define KBC_OUT_BUF 0x60
#define KBC_IN_BUF 0x60
#define KBC_DATA_REG 0x60
#define KBC_STAT_REG 0x64
#define KBC_CMD_REG 0x64
#define KBC_HOOK_BIT 1
#define BREAK_ESC_CODE 0x81
#define MAKE_ESC_CODE 0x01

//STATUS REGISTER
#define KBC_OBF BIT(0)
#define KBC_IBF BIT(1)
#define PAR_ERR BIT(7)
#define TO_ERR BIT(6)
#define AUX BIT(5)

//C@KBD values
#define ACK 0xFA
#define RESEND 0xFE
#define ERROR 0xFC
//==========================================================================================
/*
 * MOUSE MACROS
 */

#define MOUSE_IRQ 12

#define MOUSE_STREAM_MODE 0xEA
#define MOUSE_DISABLE_STREAM_MODE 0xF5
#define WRITE_BYTE_TO_MOUSE 0xD4
#define MOUSE_SEND_PACKETS 0xF4
#define MOUSE_STATUS_REQUEST 0xE9
#define ENABLE_MOUSE 0xA8

/* MOUSE PACKETS */

#define LEFT_BUTTON BIT(0)
#define RIGHT_BUTTON BIT(1)
#define MIDDLE_BUTTON BIT(2)
#define X_SIGN BIT(4)
#define Y_SIGN BIT(5)
#define X_OVERFLOW BIT(6)
#define Y_OVERFLOW BIT(7)

/* MOUSE STATUS */

#define RIGHT_STATUS_BUTTON BIT(0)
#define MIDDLE_STATUS_BUTTON BIT(1)
#define LEFT_STATUS_BUTTON BIT(2)
#define SCALLING BIT(4)
#define SEND_PACKETS_ENABLED BIT(5)
#define REMOTE BIT(6)

//==========================================================================================
/*
 * VBE MACROS
 */

#define MODE_640_480 		0x111
#define MODE_800_600 		0x114
#define MODE_1024_768 		0x117
#define MODE_1280_1024 		0x11A
#define VBE_CONTROLLER_INFO 	0x00
#define VBE_MODE_INFO 			0x01
#define SET_VBE_MODE 			0x02

// VBE RETURN VALUES
#define VBE_INTERRUPT	 	0x10  /* BIOS video services */
#define VBE_FUNCTION		0x4F
#define VBE_FUNCTION_SUPPORTED			0x4F
#define VBE_FUNCTION_CALL_SUCCESSFUL	0x00
#define VBE_FUNCTION_CALL_FAILED		0x01
#define VBE_FUNCTION_NOT_SUPPORTED		0x02
#define VBE_FUNCTION_CALL_INVALID		0x03

/* Constants for VBE 0x105 mode */

/* The physical address may vary from VM to VM.
 * At one time it was 0xD0000000
 *  #define VRAM_PHYS_ADDR    0xD0000000
 * Currently on lab B107 is 0xF0000000
 * Better run my version of lab5 as follows:
 *     service run `pwd`/lab5 -args "mode 0x105"
 */
#define VRAM_PHYS_ADDR	0xF0000000
#define H_RES             1024
#define V_RES		  768
#define BITS_PER_PIXEL	  8

//==========================================================================================
/*
 * COLOURS
 */

#define COLOUR_BLACK		RGB(0,0,0)
#define COLOUR_WHITE 		RGB(255,255,255)
#define COLOUR_GREY			RGB(128,128,128)

#define COLOUR_RED			RGB(255,0,0)
#define COLOUR_GREEN		RGB(0,255,0)
#define COLOUR_BLUE 		RGB(0,0,255)

#define COLOUR_YELLOW 		RGB(255,255,0)
#define COLOUR_MAGENTA		RGB(255,0,255)
#define COLOUR_CYAN			RGB(0,255,255)

#define COLOUR_DARK_RED 	RGB(128,0,0)
#define COLOUR_DARK_GREEN	RGB(0,128,0)
#define COLOUR_DARK_BLUE	RGB(0,0,128)

#define COLOUR_ORANGE		RGB(240,148,0)
#define COLOUR_BROWN		RGB(139,69,19)
#define COLOUR_PINK			RGB(255,192,203)
#define COLOUR_PURPLE		RGB(128,0,128)

//==========================================================================================

/*
 * PATHS
 */

#define PATH_RESOURCES 		"/home/lcom/lcom1516-t2g15/proj/code/resources/"
#define PATH_FONT			PATH_RESOURCES	"font/"
#define PATH_IMAGE			PATH_RESOURCES	"images/"
#define PATH_STAGE			PATH_RESOURCES	"stages/"

//==========================================================================================

#define NUM_STAGES	9
#define NUM_CURSORS	4

/**
 *  Converts RGB to Hex value.
 */
int RGB(unsigned char r, unsigned char g, unsigned char b);
/**
 * Swaps two values
 */
void swap(int* xi, int* xf);
/**
 * Round function, introduced in Minix 3.3.0
 */
double round(double x);
double ceil(double num);

const char* getMousePath(int cursor);
const char* getFontPath(char* character);
const char* getImagePath(char* image);
const char* getStagePath(int number);
#endif
