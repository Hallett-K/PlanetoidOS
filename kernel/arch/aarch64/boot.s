.section .text.boot
.global _asm_start
.type _asm_start, %function

_asm_start:

    // Enable FP/SIMD access
    mrs x0, CPACR_EL1
    orr x0, x0, #(3 << 20)
    msr CPACR_EL1, x0
    isb

    // Stack
    ldr x0, =__stack_top
    mov sp, x0

    // Exception table
    ldr x0, =exception_vector_table
    msr VBAR_EL1, x0 
    isb

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
    