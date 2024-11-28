#include <mem.h>
#include <io.h>
#include <vga.h>
#include <util.h>

#define KERNEL_START 0xC0000000
#define PAGE_PRESENT 1 << 0
#define PAGE_WRITE 1 << 1

static uint32_t pageFrameMin;
static uint32_t pageFrameMax;
static uint32_t allocated; 

#define NUM_PAGE_DIRS 256
#define NUM_PAGE_FRAMES (0x100000000 / 4096 / 8)


static uint8_t memBitmap[NUM_PAGE_FRAMES / 8]; // TEMP

static uint32_t pageDirs[NUM_PAGE_DIRS][1024] __attribute__((aligned(4096)));
static uint8_t pageDirUsed[NUM_PAGE_DIRS];

void memory_init(struct multiboot_info_t *multiboot_info)
{
    (void)multiboot_info;

    for (uint32_t i = 0; i < multiboot_info->mmap_length; i += sizeof(struct multiboot_memory_map_entry_t))
    {
        serial_write_string(SERIAL_PORT_COM1, "Slot ");
        serial_write_int(SERIAL_PORT_COM1, i);
        serial_write_string(SERIAL_PORT_COM1, ": ");

        vga_writestring("Slot ");
        vga_writeint(i);
        vga_writestring(": ");

        struct multiboot_memory_map_entry_t* entry = (struct multiboot_memory_map_entry_t*)(multiboot_info->mmap_addr + i);
        serial_write_string(SERIAL_PORT_COM1, "Memory type: ");
        vga_writestring("Memory type: ");
        switch (entry->type)
        {
            case MULTIBOOT_MEMORY_AVAILABLE:
                serial_write_string(SERIAL_PORT_COM1, "Available - ");
                vga_writestring("Available - ");
                break;
            case MULTIBOOT_MEMORY_RESERVED:
                serial_write_string(SERIAL_PORT_COM1, "Reserved - ");
                vga_writestring("Reserved - ");
                break;
            case MULTIBOOT_MEMORY_ACPI:
                serial_write_string(SERIAL_PORT_COM1, "ACPI - ");
                vga_writestring("ACPI - ");
                break;
            case MULTIBOOT_MEMORY_NVS:
                serial_write_string(SERIAL_PORT_COM1, "NVS - ");
                vga_writestring("NVS - ");
                break;
            case MULTIBOOT_MEMORY_BADRAM:
                serial_write_string(SERIAL_PORT_COM1, "Bad RAM - ");
                vga_writestring("Bad RAM - ");
                break;
        }

        serial_write_string(SERIAL_PORT_COM1, "Start: "); 
        vga_writestring("Start: ");
        uint64_t start = entry->addr_low | ((uint64_t)entry->addr_high << 32);
        serial_write_ulong(SERIAL_PORT_COM1, start);
        vga_writeint(start);
        serial_write_string(SERIAL_PORT_COM1, " - Length: ");
        vga_writestring(" - Length: ");
        uint64_t length = entry->len_low | ((uint64_t)entry->len_high << 32);
        serial_write_ulong(SERIAL_PORT_COM1, length);
        vga_writeint(length);
        serial_write_string(SERIAL_PORT_COM1, "\n");
        vga_writestring("\n");
    }

    uint32_t mod1 = *(uint32_t*)(multiboot_info->mods_addr + 0x04);
    uint32_t physicalAllocStart = (mod1 & 0xFFF) & ~0xFFF;
    uint32_t memHigh = multiboot_info->mem_upper * 1024;

    init_page_dir[0] = 0; 
    invalidate_page_dir(0);
    
    init_page_dir[1023] = ((uint32_t)init_page_dir - KERNEL_START) | PAGE_PRESENT | PAGE_WRITE;
    invalidate_page_dir(0xFFFFF000);

    // Physical memory manager initialization 
    pageFrameMin = (physicalAllocStart + 4095) / 4096;
    pageFrameMax = (memHigh + 4095) / 4096;
    allocated = 0;

    memset(memBitmap, 0, sizeof(memBitmap));
    memset(pageDirs, 0, 4096 * NUM_PAGE_DIRS);
    memset(pageDirUsed, 0, NUM_PAGE_DIRS);
}