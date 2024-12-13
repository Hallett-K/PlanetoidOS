org 0x7c00
bits 16

jmp short start 
nop

bdb_oem_name:                 db 'MSWIN4.1'
bdb_bytes_per_sector:         dw 512
bdb_sectors_per_cluster:      db 1
bdb_reserved_sectors:         dw 1
bdb_number_of_fats:           db 2
bdb_root_directory_entries:   dw 224
bdb_total_sectors:            dw 2880
bdb_media_descriptor:         db 0xF0
bdb_sectors_per_fat:          dw 9
bdb_sectors_per_track:        dw 18
bdb_number_of_heads:          dw 2
bdb_hidden_sectors:           dd 0
bdb_large_sectors:            dd 0

ebr_drive_number:             db 0
ebr_reserved:                 db 0 ; reserved
ebr_signature:                db 0x29
ebr_volume_id:                db 0, 0, 0, 0
ebr_volume_label:             db 'PLANETOID  '
ebr_filesystem_type:          db 'FAT12   '

start:
    mov ax, 0 
    mov ds, ax
    mov es, ax
    mov ss, ax 

    mov sp, 0x7C00

    mov [ebr_drive_number], dl ; Drive number
    mov ax, 1 ; LBA index
    mov cl, 1 ; Sector number in LBA
    mov bx, 0x7E00 ; Buffer address
    call read_floppy

    hlt

read_floppy:
    push ax
    push bx
    push cx 
    push dx 
    push di 

    call lba_to_chs

    mov ah, 2
    mov di, 3 ; counter

    ; begin read_attempt

read_attempt:
    stc 
    int 0x13 
    jnc read_success 

    call reset_floppy

    dec di 
    test di, di 
    jnz read_attempt

    ; begin read_fail

read_fail:
    hlt
    jmp $ ; TODO: Display error message

read_success:
    pop di
    pop dx 
    pop cx 
    pop bx 
    pop ax 

    ret

reset_floppy:
    pusha
    mov ah, 0
    stc
    int 0x13
    jc read_fail
    popa
    ret

; input: LBA (ax)
; output: cx [bits 0-5] sector number, cx [bits 6-15] cylinder number, dh head number
lba_to_chs:
    push ax 
    push dx 

    xor dx, dx 

    ; (LBA (ax) % sectors per track) + 1 = sector
    div word [bdb_sectors_per_track] ; ax = LBA (ax) / sectors per track, dx = LBA (ax) % sectors per track
    inc dx ; sector is now in dx
    mov cx, dx 

    xor dx, dx
    ; (LBA (ax) / sectors per track) % number of heads = head
    ; (LBA (ax) / sectors per track) / number of heads = cylinder
    div word [bdb_number_of_heads]

    mov dh, dl ; head is now in dh - dl is the lower 8 bits of the result of the division
    mov ch, al 
    shl ah, 6
    or cl, ah ; cylinder now in cl

    pop ax 
    mov dl, al 
    pop ax 

    ret 


file_kernel: db 'KERNEL  SYS'
file_kernel_cluster: dw 0
kernel_load_segment EQU 0x2000
kernel_load_offset EQU 0x0000

times 510 - ($ - $$) db 0
dw 0xaa55

buffer:
    times 