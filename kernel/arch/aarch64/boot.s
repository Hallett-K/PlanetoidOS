.section .text.boot
.global _asm_start
.type _asm_start, %function

_asm_start:

    // Stack
    ldr x0, =__stack_top
    mov sp, x0

    // Clear BSS
    ldr x0, =__bss_start
    ldr x1, =__bss_end

bss_clear:
    cmp x0, x1 
    b.hs bss_cleared

    str xzr, [x0]
    add x0, x0, #8
    b bss_clear

bss_cleared:
    bl kernel_main

1:
    wfe
    b 1b
    