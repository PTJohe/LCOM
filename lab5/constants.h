#ifndef __CONSTANTS_H
#define __CONSTANTS_H

#define BIT(n) (0x01 << n)

//==========================================================================================
/*
 * TIMER MACROS
 */

#define TIMER0_IRQ 0    /**< @brief Timer 0 IRQ line */
#define TIMER_FREQ  1193182    /**< @brief Clock frequency for timer in PC and AT */
#define TIMER_DEFAULT_FREQ 60 /**< @brief Default interrupt frequency of timer = 60 Hz */

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

//C@KBD values
#define ACK 0xFA
#define RESEND 0xFE
#define ERROR 0xFC

//==========================================================================================
/*
 * VBE MACROS
 */

#define MODE_640_480 		0x101
#define MODE_800_600 		0x103
#define MODE_1024_768 		0x105
#define MODE_1280_1024 		0x107
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
#endif
