cmake_minimum_required(VERSION 3.25)

project(PlanetoidOS LANGUAGES C CXX ASM)

set(PLATFORM "virt" CACHE STRING "Target platform (virt or pi5)")
set_property(CACHE PLATFORM PROPERTY STRINGS virt pi5)

# TODO: Common sources

set(COMMON_SOURCES
    kernel/arch/aarch64/gic.cpp
    kernel/core/interrupts.cpp
    kernel/core/log.cpp
    kernel/drivers/uart.c
    kernel/memory/memory_map.cpp
    kernel/memory/mmu.cpp
    kernel/memory/physical_memory.cpp
)

if (PLATFORM STREQUAL "pi5")
    set(PLATFORM_SOURCES
        kernel/arch/aarch64/pi/boot.s
        kernel/drivers/pi/status_led.c
        kernel/kernel_pi5.cpp
    )
    set(LINKER_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/linker/kernel-pi.ld)
    set(BUILD_NAME "kernel-pi5")
elseif (PLATFORM STREQUAL "virt")
    set(PLATFORM_SOURCES
    kernel/arch/aarch64/boot.s 
    kernel/arch/aarch64/context_switch.s
    kernel/arch/aarch64/exception.cpp 
    kernel/arch/aarch64/exception.s 
    kernel/arch/aarch64/timer.cpp 
    kernel/kernel.cpp 
    kernel/core/task/mutex.cpp
    kernel/core/task/scheduler.cpp
    kernel/core/task/task.cpp
    kernel/core/task/wait_queue.cpp
    kernel/memory/kernel_heap.cpp
    kernel/memory/virtual_memory.cpp
    )
    set (LINKER_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/linker/kernel-virt.ld)
    set(BUILD_NAME "kernel-virt")
else()
    message(FATAL_ERROR "Unknown Platform: ${PLATFORM}")
endif()

add_executable(PlanetoidOS 
    ${PLATFORM_SOURCES}
    ${COMMON_SOURCES}
    )

if (PLATFORM STREQUAL "pi5")
    target_compile_definitions(PlanetoidOS PRIVATE OS_PLATFORM_PI5=1)
elseif(PLATFORM STREQUAL "virt")
    target_compile_definitions(PlanetoidOS PRIVATE OS_PLATFORM_VIRT=1)
endif()

target_compile_options(PlanetoidOS PRIVATE -Wall -Wextra)
target_link_options(PlanetoidOS PRIVATE -T${LINKER_SCRIPT} -nostdlib -nodefaultlibs -nostartfiles)
target_include_directories(PlanetoidOS PRIVATE kernel)

set_target_properties(PlanetoidOS PROPERTIES OUTPUT_NAME ${BUILD_NAME})

if (PLATFORM STREQUAL "pi5")
    set(PI5_IMAGE ${CMAKE_CURRENT_BINARY_DIR}/kernel-pi5)

    set(PI5_BOOT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/out/boot)
    set(PI5_BOOT_CONFIG ${PI5_BOOT_DIR}/config.txt)
    set(PI5_BOOT_IMAGE ${PI5_BOOT_DIR}/kernel-pi5) 

    set_target_properties(PlanetoidOS PROPERTIES OUTPUT_NAME "kernel-pi5" SUFFIX ".elf")
    add_custom_command(OUTPUT ${PI5_IMAGE} 
        COMMAND ${CMAKE_OBJCOPY} -O binary $<TARGET_FILE:PlanetoidOS> ${PI5_IMAGE} DEPENDS PlanetoidOS COMMENT "Creating Pi 5 Kernel Image" VERBATIM)
    add_custom_target(kernel-pi5-image ALL DEPENDS ${PI5_IMAGE})
    add_custom_command(OUTPUT ${PI5_BOOT_IMAGE} ${PI5_BOOT_CONFIG} 
        COMMAND ${CMAKE_COMMAND} -E make_directory ${PI5_BOOT_DIR} 
        COMMAND ${CMAKE_COMMAND} -E copy ${PI5_IMAGE} ${PI5_BOOT_IMAGE}
        COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/boot/pi/config.txt ${PI5_BOOT_CONFIG} DEPENDS kernel-pi5-image ${CMAKE_CURRENT_SOURCE_DIR}/boot/pi/config.txt COMMENT "Staging Pi 5 Boot files" VERBATIM)
    add_custom_target(pi5-boot ALL DEPENDS ${PI5_BOOT_IMAGE} ${PI5_BOOT_CONFIG})
endif()