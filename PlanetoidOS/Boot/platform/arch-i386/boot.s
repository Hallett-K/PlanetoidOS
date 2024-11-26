/* Declare constants for the multiboot header. */
.set ALIGN,    1<<0             /* align loaded modules on page boundaries */
.set MEMINFO,  1<<1             /* provide memory map */
.set FLAGS,    ALIGN | MEMINFO  /* this is the Multiboot 'flag' field */
.set MAGIC,    0x1BADB002       /* 'magic number' lets bootloader find the header */
.set CHECKSUM, -(MAGIC + FLAGS) /* checksum of above, to prove we are multiboot */

.section .multiboot, "aw"
.align 4
.long MAGIC
.long FLAGS
.long CHECKSUM

.section .bss
.align 16
stack_bottom:
.skip 16384 # 16 KiB
stack_top:

.section .boot, "a"
.global _start
.type _start, @function
_start:
	mov $(init_page_dir - 0xC0000000), %ecx
	mov %ecx, %cr3

	mov %cr4, %ecx
	or $0x00000010, %ecx
	mov %ecx, %cr4

	mov %cr0, %ecx
	or $0x80000000, %ecx
	mov %ecx, %cr0

	jmp higher_half

.section .text 
higher_half:
	mov $stack_top, %esp

	push %eax
	push %ebx
	call kernel_early_init

	call kernel_main

	cli
1:	hlt
	jmp 1b

.section .data 
.align 4096
.global init_page_dir 
init_page_dir:
	.long 0b10000011 
	.skip 3068 

	.long (0 << 22) | 0b10000011
	.long (1 << 22) | 0b10000011
	.long (2 << 22) | 0b10000011
	.long (3 << 22) | 0b10000011
	.skip 1008 
