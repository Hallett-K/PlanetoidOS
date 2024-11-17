.global gdt_load

gdt_load:
    cli
    mov 4(%esp), %eax
    lgdt (%eax)
    
    mov $0x10, %ax
    mov %ax, %ds
    mov %ax, %es
    mov %ax, %fs
    mov %ax, %gs
    mov %ax, %ss

    mov %cr0, %eax
    or $1, %eax
    mov %eax, %cr0

    jmp $0x08, $gdt_ret
    
gdt_ret:
    sti
    ret