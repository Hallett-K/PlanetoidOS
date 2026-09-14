.section .text.boot
.global _asm_start
.type _asm_start, %function

_asm_start:
    bl kernel_main

1:
    wfe
    b 1b