cmake_minimum_required(VERSION 3.25)

project(PlanetoidOS LANGUAGES C CXX ASM)

add_executable(PlanetoidOS 
    kernel/arch/aarch64/boot.s 
    kernel/arch/aarch64/exception.cpp 
    kernel/arch/aarch64/exception.s 
    kernel/arch/aarch64/gic.cpp 
    kernel/arch/aarch64/timer.cpp 
    kernel/kernel.cpp 
    kernel/drivers/uart.c 
    kernel/core/interrupts.cpp 
    kernel/core/log.cpp
    kernel/memory/memory_map.cpp 
    kernel/memory/mmu.cpp
    kernel/memory/physical_memory.cpp)

target_compile_options(PlanetoidOS PRIVATE -Wall -Wextra)
target_link_options(PlanetoidOS PRIVATE -T${CMAKE_SOURCE_DIR}/linker/kernel.ld -nostdlib -nodefaultlibs -nostartfiles)
target_include_directories(PlanetoidOS PRIVATE kernel)

set_target_properties(PlanetoidOS PROPERTIES OUTPUT_NAME "kernel")