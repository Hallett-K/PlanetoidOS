.section .text
.global task_switch
.type task_switch, %function

task_switch:
    stp x19, x20, [x0, #152]
    stp x21, x22, [x0, #168]
    stp x23, x24, [x0, #184]
    stp x25, x26, [x0, #200]
    stp x27, x28, [x0, #216]
    stp x29, x30, [x0, #232]

    mov x2, sp
    str x2, [x0, #248]

    adr x2, .Lresume
    str x2, [x0, #256]

    ldp x19, x20, [x1, #152]
    ldp x21, x22, [x1, #168]
    ldp x23, x24, [x1, #184]
    ldp x25, x26, [x1, #200]
    ldp x27, x28, [x1, #216]
    ldp x29, x30, [x1, #232]

    ldr x2, [x1, #248]
    mov sp, x2 

    ldr x2, [x1, #256]

    msr daifclr, #2
    isb

    br x2

.Lresume:
    ret

.global task_entry_trampoline
.type task_entry_trampoline, %function

task_entry_trampoline:
    mov x0, x19
    blr x0

    bl task_terminated

1:
    wfi
    b 1b
    