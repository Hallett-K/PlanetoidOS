#include <util.h>

void* memset(void* ptr, char val, uint32_t num)
{
    char* p = (char*)ptr;
    for (uint32_t i = 0; i < num; i++)
        p[i] = val;
    return ptr;
}