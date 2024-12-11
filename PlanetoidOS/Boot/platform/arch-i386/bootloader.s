org 0x7c00
bits 16

jmp start 

db 'MSWIN4.1' ; OEM name
dw 512 ; bytes per sector
db 1 ; sectors per cluster
dw 1 ; reserved sectors
db 2 ; number of FATs
dw 224 ; number of root directory entries
dw 2880 ; total sectors
db 0xF8 ; media descriptor
dw 9 ; sectors per FAT
dw 18 ; sectors per track
dw 2 ; number of heads
dw 0 ; hidden sectors
db 0 ; drive number
db 0 ; reserved
db 0x29 ; signature
db 0, 0, 0, 0 ; volume ID
db 'FAT12   ' ; volume label

start:
    mov ah, 0x0e
    mov al, 'H'
    int 0x10
    jmp start

times 510 - ($ - $$) db 0
dw 0xaa55