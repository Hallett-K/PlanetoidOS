#ifndef _PL_MEMORY_H
#define _PL_MEMORY_H

#include <stdint.h>

struct multiboot_info_t
{
    uint32_t flags;
    uint32_t mem_lower;
    uint32_t mem_upper;
    uint32_t boot_device;
    uint32_t cmdline;
    uint32_t mods_count;
    uint32_t mods_addr;
    uint32_t syms[4];
    uint32_t mmap_length;
    uint32_t mmap_addr;
    uint32_t drives_length;
    uint32_t drives_addr;
    uint32_t config_table;
    uint32_t boot_loader_name;
    uint32_t apm_table;
    uint32_t vbe_control_info;
    uint32_t vbe_mode_info;
    uint16_t vbe_mode;
    uint16_t vbe_interface_seg;
    uint16_t vbe_interface_off;
    uint16_t vbe_interface_len;
    uint32_t framebuffer_addr_low;
    uint32_t framebuffer_addr_high;
    uint32_t framebuffer_pitch;
    uint32_t framebuffer_width;
    uint32_t framebuffer_height;
    uint8_t framebuffer_bpp;
    uint8_t framebuffer_type;
    uint8_t color_info[4];
} __attribute__((packed));

struct multiboot_memory_map_entry_t
{
    uint32_t size;
    uint32_t addr_low;
    uint32_t addr_high;
    uint32_t len_low;
    uint32_t len_high;

#define MULTIBOOT_MEMORY_AVAILABLE 1
#define MULTIBOOT_MEMORY_RESERVED 2
#define MULTIBOOT_MEMORY_ACPI 3
#define MULTIBOOT_MEMORY_NVS 4
#define MULTIBOOT_MEMORY_BADRAM 5

    uint32_t type;
} __attribute__((packed));

void memory_init(struct multiboot_info_t *multiboot_info);

#endif // _PL_MEMORY_H