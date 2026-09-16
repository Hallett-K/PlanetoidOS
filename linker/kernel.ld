ENTRY(_asm_start)

SECTIONS
{
    . = 0x40080000;

    . = ALIGN(4096);
    .text :
    {
        __text_start = .;
        KEEP(*(.text.boot))
        *(.text*)
        __text_end = .;
    }

    . = ALIGN(4096);
    .rodata :
    {
        __rodata_start = .;
        *(.rodata*)
        __rodata_end = .;
    }

    . = ALIGN(4096);
    .data :
    {
        __data_start = .;
        *(.data*)
        __data_end = .;
    }

    . = ALIGN(4096);
    .bss :
    {
        __bss_start = .;
        *(.bss*)
        *(COMMON)

        . = ALIGN(4096);
        __physical_bitmap_start = .;

        . += 0x20000;

        __physical_bitmap_end = .;

        __bss_end = .;

        . = ALIGN(4096);
        __stack_bottom = .;

        . += 0x10000;
        __stack_top = .;

    }
}