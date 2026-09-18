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

    add x0, sp, #(36 * 8)
    str x0, [sp, #(35 * 8)]

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

    add x0, sp, #(36 * 8)
    str x0, [sp, #(35 * 8)]

    mov x0, sp
    bl irq_exception_handler
    
    mov x30, sp

    ldr x0, [x30, #(31 * 8)]
    msr ELR_EL1, x0

    ldr x0, [x30, #(34 * 8)]
    msr SPSR_EL1, x0

    ldr x16, [x30, #(35 * 8)]

    ldp x0, x1, [x30, #(0 * 16)]
    ldp x2, x3, [x30, #(1 * 16)]
    ldp x4, x5, [x30, #(2 * 16)]
    ldp x6, x7, [x30, #(3 * 16)]
    ldp x8, x9, [x30, #(4 * 16)]
    ldp x10, x11, [x30, #(5 * 16)]
    ldp x12, x13, [x30, #(6 * 16)]
    ldp x14, x15, [x30, #(7 * 16)]

    ldp x17, x18, [x30, #(17 * 8)]
    ldp x19, x20, [x30, #(19 * 8)]
    ldp x21, x22, [x30, #(21 * 8)]
    ldp x23, x24, [x30, #(23 * 8)]
    ldp x25, x26, [x30, #(25 * 8)]
    ldp x27, x28, [x30, #(27 * 8)]
    ldr x29, [x30, #(29 * 8)]

    mov sp, x16

    ldr x16, [x30, #(16 * 8)]
    ldr x30, [x30, #(30 * 8)]

    eret

exception_fiq:
    b fiq_exception_handler

exception_serror:
    b serror_exception_handler
