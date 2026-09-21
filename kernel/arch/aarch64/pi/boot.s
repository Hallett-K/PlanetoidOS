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
    mrs x0, hcr_el2
    orr x0, x0, #(1 << 31)
    msr hcr_el2, x0

    ldr x0, =0x30D00800
    msr sctlr_el1, x0
    isb

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

    // Clear bss
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

halt:
    wfe
    b halt
    