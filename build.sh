#! /bin/bash

PATH="$HOME/opt/cross/bin:$PATH"

INCLUDE_VGA_DIR="PlanetoidOS/VGA/include"

DEV_BOOT_DIR="PlanetoidOS/Boot/platform/arch-i386"
DEV_VGA_DIR="PlanetoidOS/VGA/platform/arch-i386"

BOOT_OBJ="$DEV_BOOT_DIR/boot.o"
KERNEL_OBJ="$DEV_BOOT_DIR/kernel.o"
VGA_OBJ="$DEV_VGA_DIR/vga.o"

CRTBEGIN=$(i686-elf-gcc -print-file-name=crtbegin.o)
CRTEND=$(i686-elf-gcc -print-file-name=crtend.o)

rm -f $BOOT_OBJ $KERNEL_OBJ PlanetoidOS.bin PlanetoidOS.iso

i686-elf-as -g "$DEV_BOOT_DIR/boot.s" -o "$BOOT_OBJ"

i686-elf-gcc -g -c "$DEV_VGA_DIR/vga.c" -o "$VGA_OBJ" -I"$INCLUDE_VGA_DIR" -std=gnu99 -ffreestanding -O0 -Wall -Wextra
i686-elf-gcc -g -c "$DEV_BOOT_DIR/kernel.c" -o "$KERNEL_OBJ" -I"$INCLUDE_VGA_DIR" -std=gnu99 -ffreestanding -O0 -Wall -Wextra
i686-elf-gcc -g -T "$DEV_BOOT_DIR/linker.ld" -o "PlanetoidOS.bin" -ffreestanding -O0 -nostdlib "$BOOT_OBJ" "$KERNEL_OBJ" "$VGA_OBJ" -lgcc

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

# Copy to mnt/d/projects/OS
#cp PlanetoidOS.iso /mnt/d/projects/OS
#cp PlanetoidOS.bin /mnt/d/projects/OS