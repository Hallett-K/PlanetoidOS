# PlanetoidOS

Hobbyist OS made for Raspberry Pi 5 (eventually - currently building with qemu virt)


## Installation

Build with CMake

```bash
  cmake -B build -DCMAKE_TOOLCHAIN_FILE=cmake/aarch64-none-elf.cmake
  cmake --build build
```

Run via qemu:

```bash
  qemu-system-aarch64 -machine virt -cpu cortex-a53 -m 4G -nographic -kernel build/kernel
```

Pi 5 build script coming soon!