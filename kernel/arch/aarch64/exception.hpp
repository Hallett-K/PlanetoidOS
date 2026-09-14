#pragma once 

#include <cstdint>

struct exception_context
{
    uint64_t x[31];

    uint64_t elr;
    uint64_t esr;
    uint64_t far;
    uint64_t spsr;
};