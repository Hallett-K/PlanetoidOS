# PlanetoidOS

Hobbyist OS made for Raspberry Pi 5


## Installation

Build with CMake


QEMU:
```bash
  cmake -B build -DCMAKE_TOOLCHAIN_FILE=cmake/aarch64-none-elf.cmake -DPLATFORM=virt
  cmake --build build
```

Pi 5:
```bash
  cmake -B build -DCMAKE_TOOLCHAIN_FILE=cmake/aarch64-none-elf.cmake -DPLATFORM=pi5
  cmake --build build
```

Run via qemu:

```bash
  qemu-system-aarch64 -machine virt -cpu cortex-a53 -m 4G -nographic -kernel build/kernel
```

Run via Pi 5:

The Raspberry Pi 5 build produces the following structure from the repo root:

out/boot/
- config.txt
- kernel-pi5

These files need to be copied to the boot partition of a bootable Micro SD Card prepared with Raspberry Pi imager:

SD Card/
- config.txt
- kernel-pi5
- ...

The existing Raspberry Pi 5 Boot files should be left in place.

NOTE: PlanetoidOS is currently under development and Raspberry Pi 5 support is not up to date with the QEMU build. The kernel may not yet provide visible output.