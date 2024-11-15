.global _io_inb
.global _io_outb
.global _io_wait

_io_inb:
    mov 4(%esp), %dx
    inb %dx, %al
    ret

_io_outb:
    mov 8(%esp), %al
    mov 4(%esp), %dx
    outb %al, %dx
    ret

_io_wait:
    outb %al, $0x80
    ret
