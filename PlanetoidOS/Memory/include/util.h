#ifndef _PL_MEM_UTIL_H
#define _PL_MEM_UTIL_H

#include <stdint.h>

struct interrupt_frame
{
    uint32_t cr2;
    uint32_t ds;
    uint32_t edi;
    uint32_t esi;
    uint32_t ebp;
    uint32_t esp;
    uint32_t ebx;
    uint32_t edx;
    uint32_t ecx;
    uint32_t eax;
    uint32_t interrupt_number;
    uint32_t error_code;
    uint32_t eip;
    uint32_t cs;
    uint32_t eflags;
    uint32_t user_esp;
    uint32_t ss;
} __attribute__((packed));

void* memset(void* ptr, char val, uint32_t num); // TEMP

#endif