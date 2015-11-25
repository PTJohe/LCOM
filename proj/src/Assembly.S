#define KBC_OUT_BUF 0x60

.global _keyboard_read_scancode

.text

_keyboard_read_scancode:

xor %eax, %eax
inb $KBC_OUT_BUF
ret
