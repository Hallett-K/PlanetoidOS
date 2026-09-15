.section .text.exceptions
.align 11
.global exception_vector_table
.type exception_vector_table, %function

exception_vector_table:
    b exception_sync
    .space 124

    b exception_irq
    .space 124

    b exception_fiq
    .space 124

    b exception_serror
    .space 124

    b exception_sync
    .space 124

    b exception_irq
    .space 124

    b exception_fiq
    .space 124

    b exception_serror
    .space 124

    b exception_sync
    .space 124

    b exception_irq
    .space 124

    b exception_fiq
    .space 124

    b exception_serror
    .space 124

    b exception_sync
    .space 124

    b exception_irq
    .space 124

    b exception_fiq
    .space 124

    b exception_serror
    .space 124

exception_sync:
    sub sp, sp, #(36 * 8)

    stp x0, x1, [sp, #(0 * 16)]
    stp x2, x3, [sp, #(1 * 16)]
    stp x4, x5, [sp, #(2 * 16)]
    stp x6, x7, [sp, #(3 * 16)]
    stp x8, x9, [sp, #(4 * 16)]
    stp x10, x11, [sp, #(5 * 16)]
    stp x12, x13, [sp, #(6 * 16)]
    stp x14, x15, [sp, #(7 * 16)]
    stp x16, x17, [sp, #(8 * 16)]
    stp x18, x19, [sp, #(9 * 16)]
    stp x20, x21, [sp, #(10 * 16)]
    stp x22, x23, [sp, #(11 * 16)]
    stp x24, x25, [sp, #(12 * 16)]
    stp x26, x27, [sp, #(13 * 16)]
    stp x28, x29, [sp, #(14 * 16)]
    str x30, [sp, #(30 * 8)]

    mrs x0, ELR_EL1
    str x0, [sp, #(31 * 8)]

    mrs x0, ESR_EL1
    str x0, [sp, #(32 * 8)]

    mrs x0, FAR_EL1
    str x0, [sp, #(33 * 8)]

    mrs x0, SPSR_EL1
    str x0, [sp, #(34 * 8)]

    mov x0, sp
    b sync_exception_handler
    b .

exception_irq:
    sub sp, sp, #(36 * 8)

    stp x0, x1, [sp, #(0 * 16)]
    stp x2, x3, [sp, #(1 * 16)]
    stp x4, x5, [sp, #(2 * 16)]
    stp x6, x7, [sp, #(3 * 16)]
    stp x8, x9, [sp, #(4 * 16)]
    stp x10, x11, [sp, #(5 * 16)]
    stp x12, x13, [sp, #(6 * 16)]
    stp x14, x15, [sp, #(7 * 16)]
    stp x16, x17, [sp, #(8 * 16)]
    stp x18, x19, [sp, #(9 * 16)]
    stp x20, x21, [sp, #(10 * 16)]
    stp x22, x23, [sp, #(11 * 16)]
    stp x24, x25, [sp, #(12 * 16)]
    stp x26, x27, [sp, #(13 * 16)]
    stp x28, x29, [sp, #(14 * 16)]
    str x30, [sp, #(30 * 8)]

    mrs x0, ELR_EL1
    str x0, [sp, #(31 * 8)]

    mrs x0, ESR_EL1
    str x0, [sp, #(32 * 8)]

    mrs x0, FAR_EL1
    str x0, [sp, #(33 * 8)]

    mrs x0, SPSR_EL1
    str x0, [sp, #(34 * 8)]

    mov x0, sp
    bl irq_exception_handler
    
    ldr x0, [sp, #(31 * 8)]
    msr ELR_EL1, x0

    ldr x0, [sp, #(34 * 8)]
    msr SPSR_EL1, x0

    ldp x0, x1, [sp, #(0 * 16)]
    ldp x2, x3, [sp, #(1 * 16)]
    ldp x4, x5, [sp, #(2 * 16)]
    ldp x6, x7, [sp, #(3 * 16)]
    ldp x8, x9, [sp, #(4 * 16)]
    ldp x10, x11, [sp, #(5 * 16)]
    ldp x12, x13, [sp, #(6 * 16)]
    ldp x14, x15, [sp, #(7 * 16)]
    ldp x16, x17, [sp, #(8 * 16)]
    ldp x18, x19, [sp, #(9 * 16)]
    ldp x20, x21, [sp, #(10 * 16)]
    ldp x22, x23, [sp, #(11 * 16)]
    ldp x24, x25, [sp, #(12 * 16)]
    ldp x26, x27, [sp, #(13 * 16)]
    ldp x28, x29, [sp, #(14 * 16)]
    ldr x30, [sp, #(30 * 8)]

    add sp, sp, #(36 * 8)

    eret

exception_fiq:
    b exception_fiq

exception_serror:
    b exception_serror
