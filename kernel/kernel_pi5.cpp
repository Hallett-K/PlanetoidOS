#include <cstdint>

#include "arch/aarch64/gic.hpp"
#include "core/interrupts.hpp"
#include "core/log.hpp"
#include "drivers/pi/status_led.h"
#include "drivers/uart.h"
#include "memory/kernel_heap.hpp"
#include "memory/mmu.hpp"
#include "memory/physical_memory.hpp"
#include "memory/virtual_memory.hpp"

[[noreturn]] void fail_test()
{
    while (true)
    {
        asm volatile("wfe");
    }
}

void pos_assert(bool condition)
{
    if (!condition)
    {
        fail_test();
    }
}

extern "C" void kernel_main()
{
    status_led_init();
    status_led_on();

    uart_init();

    Interrupts::init();
    GIC::init();

    MMU::init();
    MMU::enable();
    
    PhysicalMemory::init();
    VirtualMemory::init();
    KernelHeapAllocator::init();

    void* a = kmalloc(64);
    pos_assert(a != nullptr);

    void* b = kmalloc(128);
    pos_assert(b != nullptr);
    pos_assert(b != a);

    uint8_t* a_bytes = (uint8_t*)a;
    uint8_t* b_bytes = (uint8_t*)b;

    for (uint64_t i = 0; i < 64; i++)
    {
        a_bytes[i] = (uint8_t)i;
    }

    for (uint64_t i = 0; i < 128; i++)
    {
        b_bytes[i] = (uint8_t)(255 - i);
    }

    for (uint64_t i = 0; i < 64; i++)
    {
        pos_assert(a_bytes[i] == (uint8_t)i);
    }

    for (uint64_t i = 0; i < 128; i++)
    {
        pos_assert(b_bytes[i] == (uint8_t)(255 - i));
    }

    kfree(a);

    void* c = kmalloc(32);
    pos_assert(c != nullptr);
    pos_assert(c == a);

    kfree(b);
    kfree(c);

    void* large = kmalloc(8192);
    pos_assert(large != nullptr);

    uint8_t* large_bytes = (uint8_t*)large;

    large_bytes[0] = 0x12;
    large_bytes[4096] = 0x34;
    large_bytes[8191] = 0x56;

    pos_assert(large_bytes[0] == 0x12);
    pos_assert(large_bytes[4096] == 0x34);
    pos_assert(large_bytes[8191] == 0x56);

    kfree(large);

    status_led_off();

    while (true)
    {
        asm volatile("wfe");
    }
}