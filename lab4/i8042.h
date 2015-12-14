#ifndef _I8042_H
#define _I8042_H

#define BIT(n) (0x01 << n)

#define TIMER0_IRQ 0    /**< @brief Timer 0 IRQ line */
#define KBC_IRQ 1 /**< @brief Keyboard IRQ line */
#define MOUSE_IRQ 12 /**< @brief Mouse IRQ line */


#define KBC_OUT_BUF 0x60
#define KBC_IN_BUF 0x60
#define KBC_STAT_REG 0x64
#define KBC_CMD_REG 0x64
#define KBC_HOOK_BIT 1
#define BREAK_ESC_CODE 0x81
#define MAKE_ESC_CODE 0x01

/* STATUS REGISTER */

#define KBC_OBF BIT(0)
#define KBC_IBF BIT(1)
#define PAR_ERR BIT(7)
#define TO_ERR BIT(6)
#define AUX BIT(5)

/* C@KBD values */

#define ACK 0xFA
#define RESEND 0xFE
#define ERROR 0xFC

#define KBD_TOGGLE_LEDS 0xED
#define DELAY_US 20000

/* MOUSE */

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

#endif
