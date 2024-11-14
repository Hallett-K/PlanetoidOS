#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include <vga.h>

void kernel_main(void) 
{
    vga_init();
	vga_writestring("PlanetoidOS v0.0");
}
