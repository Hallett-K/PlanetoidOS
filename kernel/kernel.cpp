#include "arch/aarch64/exception.hpp"
#include "arch/aarch64/gic.hpp"
#include "arch/aarch64/timer.hpp"
#include "core/interrupts.hpp"
#include "core/log.hpp"
#include "core/task/scheduler.hpp"
#include "core/task/task.hpp"
#include "memory/kernel_heap.hpp"
#include "memory/mmu.hpp"
#include "memory/physical_memory.hpp"
#include "memory/virtual_memory.hpp"


void disable_interrupts()
{
    asm volatile("msr daifset, #2"
        : 
        :
        : "memory");

    asm volatile("isb");
}

void enable_interrupts()
{
    asm volatile("msr daifclr, #2"
        : 
        :
        : "memory");

    asm volatile("isb");
}

void halt()
{
    while (true)
    {
        asm volatile("wfe");
    }
}

void task_a_func()
{
    Log::info("Task A Started");

    for (uint32_t i = 0; i < 5; i++)
    {
        Log::info_s("Task A Iteration %u", i);
        Timer::delay(20);
    }

    Log::info("Task A Terminating");
    Scheduler::terminate_current();
}

void task_b_func()
{
    Log::info("Task B Started");

    for (uint32_t i = 0; i < 5; i++)
    {
        Log::info_s("Task B Iteration %u", i);
        Timer::delay(30);
    }

    Log::info("Task B Terminating");
    Scheduler::terminate_current();
}

extern "C" void kernel_main()
{
    Log::info("PlanetoidOS");
    Log::info("UART Initialised.");

    Interrupts::init();
    GIC::init();
    Log::info("GIC initialised!");

    
    MMU::init();
    Log::info("MMU Enabled!");
    
    PhysicalMemory::init();
    VirtualMemory::init();
    KernelHeapAllocator::init();
    
    Log::info("Memory initialised");
    
    Scheduler::init();
    
    Log::info("Scheduler initialised");

    Task::TaskState* task_a = Task::create_task(task_a_func, 4096);
    Task::TaskState* task_b = Task::create_task(task_b_func, 4096);

    Scheduler::add_task(task_a);
    //Scheduler::add_task(task_b);
    
    GIC::enable_interrupt(30); // Timer interrupt
    Timer::init(100); // ticks every 1/100th of a second
    enable_interrupts();
    
    Scheduler::start();

    halt();
}