#include "kernel_heap.hpp"

#include "virtual_memory.hpp"

namespace 
{
    const uint64_t PAGE_SIZE = 4096;
    
    struct Block
    {
        uint64_t size;
        bool free;
        bool large;
        Block* next;
    };

    Block* first_block = nullptr;
}

namespace
{
    uint64_t align_size(uint64_t size)
    {
        return (size + 7) & ~7ULL;
    }

    Block* create_block()
    {
        const uint64_t address = VirtualMemory::allocate_page();
        if (address == 0)
        {
            return nullptr;
        }

        Block* block = (Block*)address;
        
        block->size = PAGE_SIZE - sizeof(Block);
        block->free = true;
        block->large = false;
        block->next = nullptr;

        return block;
    }

    void* allocate_from_block(Block* block, uint64_t size)
    {
        if (block == nullptr || !block->free || block->size < size)
        {
            return nullptr;
        }

        if (block->size >= size + sizeof(Block) + 8)
        {
            const uint64_t current_size = block->size;
            Block* next = block->next;

            block->free = false;
            block->size = size;
            block->next = (Block*)((uint8_t*)(block + 1) + size);

            block->next->size = current_size - size - sizeof(Block);
            block->next->free = true;
            block->next->next = next;
        }
        else
        {
            block->free = false;
        }

        return (void*)(block + 1);
    }
}

void KernelHeapAllocator::init()
{
    first_block = create_block();
}

void* kmalloc(uint64_t size)
{
    if (size == 0)
    {
        return nullptr;
    }

    size = align_size(size);

    if (size > UINT64_MAX - sizeof(Block))
    {
        return nullptr;
    }

    if (size > PAGE_SIZE - sizeof(Block))
    {
        const uint64_t total_size = size + sizeof(Block);
        const uint64_t page_count = (total_size / PAGE_SIZE) + ((total_size % PAGE_SIZE) != 0);

        const uint64_t address = VirtualMemory::allocate_pages(page_count);
        if (address == 0)
        {
            return nullptr;
        }

        Block* block = (Block*)address;

        block->size = size;
        block->free = false;
        block->large = true;
        block->next = nullptr;

        return (void*)(block + 1);
    }

    Block* previous = nullptr;
    Block* block = first_block;

    while (block != nullptr)
    {
        if (block->free && block->size >= size)
        {
            return allocate_from_block(block, size);
        }

        previous = block;
        block = block->next;
    }

    Block* new_block = create_block();
    if (new_block == nullptr)
    {
        return nullptr;
    }

    if (previous == nullptr)
    {
        first_block = new_block;
    }
    else
    {
        previous->next = new_block;
    }

    return allocate_from_block(new_block, size);
}

void kfree(void* memory)
{
    if (memory == nullptr)
    {
        return;
    }

    Block* block = ((Block*)memory) - 1;

    if (block->large)
    {
        const uint64_t total_size = block->size + sizeof(Block);
        const uint64_t page_count = (total_size / PAGE_SIZE) + ((total_size % PAGE_SIZE) != 0);
        
        VirtualMemory::free_pages((uint64_t)block, page_count);
        return;
    }

    block->free = true;

    Block* next = block->next;
    uint64_t gained_size = block->size;
    while (next != nullptr)
    {
        if (!next->free)
        {
            break;
        }

        Block* expected_next = (Block*)((uint8_t*)(block + 1) + gained_size);
        if (next != expected_next)
        {
            break;
        }

        gained_size += sizeof(Block) + next->size;
        next = next->next;
    }

    block->size = gained_size;
    block->next = next;

    Block* previous = nullptr;
    Block* current = first_block;
    while (current != nullptr && current != block)
    {
        previous = current;
        current = current->next;
    }

    if (previous != nullptr && previous->free)
    {
        Block* expected_block = (Block*)((uint8_t*)(previous + 1) + previous->size);
        if (expected_block == block)
        {
            previous->size += sizeof(Block) + block->size;
            previous->next = block->next;
        }
    }
}