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
    Log::Info("Task A Started");
    Log::Info("Task A Terminating");
}

void task_b_func()
{
    Log::Info("Task B Started");
    Log::Info("Task B Terminating");
}

extern "C" void kernel_main()
{
    Log::Info("PlanetoidOS");
    Log::Info("UART Initialised.");

    Interrupts::init();
    GIC::init();
    Log::Info("GIC initialised!");
    MMU::init();
    Log::Info("MMU Enabled!");

    

    PhysicalMemory::init();
    VirtualMemory::init();
    KernelHeapAllocator::init();

    Log::Info("Memory initialised");

    Scheduler::init();

    Log::Info("Scheduler initialised");

    Task::TaskState* task_a = Task::create_task(task_a_func, 4096);
    Task::TaskState* task_b = Task::create_task(task_b_func, 4096);

    Log::Info("Tasks created");

    Scheduler::add_task(task_a);
    Scheduler::add_task(task_b);

    Log::Info("Starting tasks");

    GIC::enable_interrupt(30); // Timer interrupt
    Timer::init(100); // ticks every 1/100th of a second
    enable_interrupts();

    Scheduler::start();

    Log::Info("Pausing for one second!");
    Timer::delay(100);
    Log::Info("Second passed!");

    halt();
}