#! /bin/bash

PATH="$HOME/opt/cross/bin:$PATH"

INCLUDE_VGA_DIR="PlanetoidOS/VGA/include"
INCLUDE_IO_DIR="PlanetoidOS/IO/include"
INCLUDE_MEMORY_DIR="PlanetoidOS/Memory/include"
INCLUDE_TIME_DIR="PlanetoidOS/Time/include"

DEV_BOOT_DIR="PlanetoidOS/Boot/platform/arch-i386"
DEV_VGA_DIR="PlanetoidOS/VGA/platform/arch-i386"
DEV_IO_DIR="PlanetoidOS/IO/platform/arch-i386"
DEV_MEMORY_DIR="PlanetoidOS/Memory/platform/arch-i386"
DEV_TIME_DIR="PlanetoidOS/Time/platform/arch-i386"
BOOT_OBJ="$DEV_BOOT_DIR/boot.o"
KERNEL_OBJ="$DEV_BOOT_DIR/kernel.o"
VGA_OBJ="$DEV_VGA_DIR/vga.o"
IO_OBJ="$DEV_IO_DIR/io.o"
SERIAL_OBJ="$DEV_IO_DIR/serial_io.o"
GDT_OBJ="$DEV_MEMORY_DIR/gdt.o"
GDT_ASM_OBJ="$DEV_MEMORY_DIR/gdt-asm.o"
IDT_OBJ="$DEV_MEMORY_DIR/idt.o"
IDT_ASM_OBJ="$DEV_MEMORY_DIR/idt-asm.o"
MEM_UTIL_OBJ="$DEV_MEMORY_DIR/util.o"
TIMER_OBJ="$DEV_TIME_DIR/timer.o"
KEYBOARD_OBJ="$DEV_IO_DIR/keyboard.o"
MEMORY_OBJ="$DEV_MEMORY_DIR/mem.o"
CRTBEGIN=$(i686-elf-gcc -print-file-name=crtbegin.o)
CRTEND=$(i686-elf-gcc -print-file-name=crtend.o)

rm -f $BOOT_OBJ $KERNEL_OBJ $VGA_OBJ $IO_OBJ $SERIAL_OBJ $GDT_OBJ $GDT_ASM_OBJ $IDT_OBJ $IDT_ASM_OBJ $MEM_UTIL_OBJ $TIMER_OBJ $KEYBOARD_OBJ $MEMORY_OBJ PlanetoidOS.bin PlanetoidOS.iso

i686-elf-as -g "$DEV_BOOT_DIR/boot.s" -o "$BOOT_OBJ"
i686-elf-as -g "$DEV_IO_DIR/io.s" -o "$IO_OBJ"
i686-elf-as -g "$DEV_MEMORY_DIR/gdt.s" -o "$GDT_ASM_OBJ"
i686-elf-as -g "$DEV_MEMORY_DIR/idt.s" -o "$IDT_ASM_OBJ"
i686-elf-gcc -g -c "$DEV_IO_DIR/serial_io.c" -o "$SERIAL_OBJ" -I"$INCLUDE_IO_DIR" -std=gnu99 -ffreestanding -O0 -Wall -Wextra
i686-elf-gcc -g -c "$DEV_VGA_DIR/vga.c" -o "$VGA_OBJ" -I"$INCLUDE_VGA_DIR" -I"$INCLUDE_IO_DIR" -std=gnu99 -ffreestanding -O0 -Wall -Wextra
i686-elf-gcc -g -c "$DEV_MEMORY_DIR/gdt.c" -o "$GDT_OBJ" -I"$INCLUDE_MEMORY_DIR" -I"$INCLUDE_IO_DIR" -std=gnu99 -ffreestanding -O0 -Wall -Wextra
i686-elf-gcc -g -c "$DEV_MEMORY_DIR/idt.c" -o "$IDT_OBJ" -I"$INCLUDE_MEMORY_DIR" -I"$INCLUDE_IO_DIR" -std=gnu99 -ffreestanding -O0 -Wall -Wextra
i686-elf-gcc -g -c "$DEV_MEMORY_DIR/util.c" -o "$MEM_UTIL_OBJ" -I"$INCLUDE_MEMORY_DIR" -std=gnu99 -ffreestanding -O0 -Wall -Wextra
i686-elf-gcc -g -c "$DEV_TIME_DIR/timer.c" -o "$TIMER_OBJ" -I"$INCLUDE_TIME_DIR" -I"$INCLUDE_MEMORY_DIR" -I"$INCLUDE_IO_DIR" -I"$INCLUDE_VGA_DIR" -std=gnu99 -ffreestanding -O0 -Wall -Wextra
i686-elf-gcc -g -c "$DEV_IO_DIR/keyboard.c" -o "$KEYBOARD_OBJ" -I"$INCLUDE_IO_DIR" -I"$INCLUDE_MEMORY_DIR" -std=gnu99 -ffreestanding -O0 -Wall -Wextra
i686-elf-gcc -g -c "$DEV_MEMORY_DIR/mem.c" -o "$MEMORY_OBJ" -I"$INCLUDE_MEMORY_DIR" -I"$INCLUDE_IO_DIR" -I"$INCLUDE_VGA_DIR" -std=gnu99 -ffreestanding -O0 -Wall -Wextra
i686-elf-gcc -g -c "$DEV_BOOT_DIR/kernel.c" -o "$KERNEL_OBJ" -I"$INCLUDE_VGA_DIR" -I"$INCLUDE_IO_DIR" -I"$INCLUDE_MEMORY_DIR" -I"$INCLUDE_TIME_DIR" -std=gnu99 -ffreestanding -O0 -Wall -Wextra
i686-elf-gcc -g -T "$DEV_BOOT_DIR/linker.ld" -o "PlanetoidOS.bin" "$BOOT_OBJ" "$KERNEL_OBJ" "$VGA_OBJ" "$IO_OBJ" "$SERIAL_OBJ" "$GDT_OBJ" "$GDT_ASM_OBJ" "$IDT_OBJ" "$IDT_ASM_OBJ" "$MEM_UTIL_OBJ" "$TIMER_OBJ" "$KEYBOARD_OBJ" "$MEMORY_OBJ" -nostdlib -lgcc

if grub-file --is-x86-multiboot PlanetoidOS.bin; then
    echo "Multiboot confirmed"
else
    echo "ERROR: The file is not multiboot"
    exit 1
fi

mkdir -p isodir/boot/grub
cp PlanetoidOS.bin isodir/boot/PlanetoidOS.bin
cp grub.cfg isodir/boot/grub/grub.cfg
grub-mkrescue -o PlanetoidOS.iso isodir

# Copy to mnt/d/projects/OS - I uncomment this locally as I develop in WSL and test on Windows
#cp PlanetoidOS.iso /mnt/d/projects/OS
#cp PlanetoidOS.bin /mnt/d/projects/OS