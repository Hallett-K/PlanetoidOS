.global idt_load 
idt_load:
    mov 4(%esp), %eax
    lidt (%eax)
    sti
    ret

.macro isr_no_error num 
    .global isr\num
    isr\num:
        cli
        push $0
        push $\num 
        jmp isr_common
.endm 

.macro isr_error num 
    .global isr\num
    isr\num:
        cli 
        push $\num
        jmp isr_common
.endm 

.macro irq num arg
    .global irq\num
    irq\num:
        cli
        push $0
        push $\arg 
        jmp irq_common
.endm

isr_no_error 0
isr_no_error 1
isr_no_error 2
isr_no_error 3
isr_no_error 4
isr_no_error 5
isr_no_error 6
isr_no_error 7
isr_error 8
isr_no_error 9
isr_error 10
isr_error 11
isr_error 12
isr_error 13
isr_error 14
isr_no_error 15
isr_no_error 16
isr_no_error 17
isr_no_error 18
isr_no_error 19
isr_no_error 20
isr_no_error 21
isr_no_error 22
isr_no_error 23
isr_no_error 24
isr_no_error 25
isr_no_error 26
isr_no_error 27
isr_no_error 28
isr_no_error 29
isr_no_error 30
isr_no_error 31

irq 0 32
irq 1 33
irq 2 34
irq 3 35
irq 4 36
irq 5 37
irq 6 38
irq 7 39
irq 8 40
irq 9 41
irq 10 42
irq 11 43
irq 12 44
irq 13 45
irq 14 46
irq 15 47

isr_no_error 128
isr_no_error 177

.extern isr_handler
isr_common:
    pusha 
    mov %ds, %eax 
    push %eax
    mov %cr2, %eax 
    push %eax 

    mov $0x10, %ax 
    mov %ax, %ds 
    mov %ax, %es 
    mov %ax, %fs 
    mov %ax, %gs 

    push %esp 
    call isr_handler

    add $8, %esp 
    pop %ebx 
    mov %bx, %ds 
    mov %bx, %es 
    mov %bx, %fs 
    mov %bx, %gs 

    popa 
    add $8, %esp 
    sti
    iret 

.extern irq_handler
irq_common:
    pusha 
    mov %ds, %eax 
    push %eax
    mov %cr2, %eax 
    push %eax 

    mov $0x10, %ax 
    mov %ax, %ds 
    mov %ax, %es 
    mov %ax, %fs 
    mov %ax, %gs 

    push %esp 
    call irq_handler

    add $8, %esp 
    pop %ebx 
    mov %bx, %ds 
    mov %bx, %es 
    mov %bx, %fs 
    mov %bx, %gs 

    popa 
    add $8, %esp 
    sti
    iret 
