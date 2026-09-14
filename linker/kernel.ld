ENTRY(_asm_start)

SECTIONS
{
    . = 0x80000;

    .text : ALIGN(4096)
    {
        KEEP(*(.text.boot))
        *(.text*)
    }

    .rodata : ALIGN(4096)
    {
        *(.rodata*)
    }

    .data : ALIGN(4096)
    {
        *(.data*)
    }

    .bss : ALIGN(4096)
    {
        __bss_start = .;
        *(.bss*)
        *(COMMON)
        __bss_end = .;
    }
}