ENTRY(_asm_start)

SECTIONS
{
    . = 0x40080000;

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

        . = ALIGN(16);
        . += 0x10000;
        __stack_top = .;

    }

    .mmu_test 0x50200000 :
    {
        __mmu_test_start = .;
        *(.mmu_test)
        __mmu_test_end = .;
    }
}