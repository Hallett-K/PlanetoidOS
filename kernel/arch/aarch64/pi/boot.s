.section .text.boot
.global _asm_start
.type _asm_start, %function

_asm_start:
    // Drop from EL2 to EL1
    mrs x1, CurrentEL
    lsr x1, x1, #2
    cmp x1, #2
    b.ne 1f

    // Configure EL1 state
    mov x0, #(0x3C5)
    msr spsr_el2, x0

    adr x0, 1f
    msr elr_el2, x0

    eret

1:
    // Disable multi-core for now
    mrs x0, mpidr_el1
    and x0, x0, #0xff

    cbz x0, primary_core

secondary_core:
    wfe
    b secondary_core

primary_core:
    ldr x0, =__stack_top
    mov sp, x0

    bl kernel_main

halt:
    wfe
    b halt
    