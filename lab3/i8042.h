#ifndef _I8042_H
#define _I8042_H

#define BIT(n) (0x01 << n)

#define KBC_IRQ 1 /**< @brief Keyboard IRQ line */

#define KBC_OUT_BUF 0x60
#define KBC_IN_BUF 0x60
#define KBC_STAT_REG 0x64
#define KBC_CMD_REG 0x64
#define KBC_HOOK_BIT 1
#define BREAK_ESC_CODE 0x81

/* STATUS REGISTER */

#define KBC_OBF BIT(0)
#define KBC_IBF BIT(1)
#define PAR_ERR BIT(7)
#define TO_ERR BIT(6)

/* C@KBD values */
#define ACK 0xFA
#define RESEND 0xFE
#define ERROR 0xFC

#define KBD_TOGGLE_LEDS 0xED
#define KBD_WAIT_TIME 20000


#endif
