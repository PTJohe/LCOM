#define KBC_OUT_BUF 0x60

.global _keyboard_int_handler_assembly

.text

_keyboard_int_handler_assembly:

xor %eax, %eax
inb $KBC_OUT_BUF
ret

