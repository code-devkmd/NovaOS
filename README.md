# NovaOS

NovaOS is a small 32-bit x86 operating system built as an educational kernel project. It is designed to demonstrate core OS concepts such as bootstrapping, interrupt handling, memory management, paging, heap allocation, a simple filesystem, and a command-line shell.

## Overview

The project boots using GRUB, initializes the CPU and system services, then starts a minimal user-facing shell. The kernel is intentionally lightweight and focuses on learning how an operating system loads, manages hardware, and exposes basic functionality.

## Features

- 32-bit kernel for x86 systems
- GRUB multiboot boot flow
- Global Descriptor Table (GDT) and Interrupt Descriptor Table (IDT)
- Interrupt service routines and PIC configuration
- Keyboard input and text terminal output
- Timer support
- Physical memory management
- Paging support
- Kernel heap allocation
- ATA/IDE disk support
- Basic filesystem implementation
- Command shell with built-in commands
- Serial output for debugging and boot messages

## System Architecture

NovaOS is organized around a classic kernel structure:

- Boot stage initializes the system and loads the kernel
- CPU infrastructure sets up segmentation and interrupts
- Device drivers handle keyboard, timer, serial, and ATA devices
- Memory subsystem manages physical memory and virtual memory
- Filesystem layer handles basic file and directory operations
- Shell provides user interaction through commands

This makes the project suitable for learning how a monolithic kernel is assembled from low-level system components.

## Folder Structure

```text
NovaOS/
├── boot/
│   └── boot.s                 # Boot assembly code
├── cpu/
│   ├── gdt.c                  # GDT setup
│   ├── gdt_flush.s            # GDT flush routine
│   ├── idt.c                  # IDT setup
│   ├── irq.c                  # IRQ handlers
│   ├── irq_stubs.s            # IRQ entry stubs
│   ├── isr.c                  # ISR logic
│   ├── isr_stubs.s            # ISR entry stubs
│   ├── pic.c                  # PIC configuration
│   └── ...
├── include/
│   ├── ata.h
│   ├── commands.h
│   ├── fs.h
│   ├── gdt.h
│   ├── heap.h
│   ├── idt.h
│   ├── io.h
│   ├── irq.h
│   ├── isr.h
│   ├── kernel.h
│   ├── keyboard.h
│   ├── memory.h
│   ├── paging.h
│   ├── panic.h
│   ├── pic.h
│   ├── serial.h
│   ├── shell.h
│   ├── string.h
│   ├── terminal.h
│   └── timer.h
├── kernel/
│   ├── ata.c
│   ├── commands.c             # Shell command implementations
│   ├── fs.c                   # Filesystem logic
│   ├── heap.c                 # Heap manager
│   ├── io.c                   # I/O helper functions
│   ├── kernel.c               # Main kernel entry point
│   ├── keyboard.c             # Keyboard driver
│   ├── memory.c               # Memory initialization
│   ├── paging.c               # Paging setup
│   ├── panic.c                # Panic handling
│   ├── serial.c               # Serial debugging output
│   ├── shell.c                # Interactive shell loop
│   ├── string.c               # String helpers
│   ├── terminal.c             # VGA terminal driver
│   └── timer.c                # Timer support
├── iso/
│   └── grub/
│       └── grub.cfg           # GRUB boot configuration
├── linker.ld                  # Linker script
├── Makefile                   # Build rules
├── README.md                  # Project documentation
└── ...
```

## Build and Run

Requirements:

- GCC or compatible C compiler
- GNU ld
- GRUB utilities
- QEMU (for running the OS in a virtual machine)

Build the kernel:

```bash
make
```

Run in QEMU:

```bash
make run
```

Clean generated files:

```bash
make clean
```

## Shell Commands

The built-in shell supports commands such as:

- help
- clear
- echo
- ls
- cd
- pwd
- mkdir
- touch
- write
- cat
- rm
- cp
- mv

These commands provide a simple interactive environment for basic file and directory operations.

## Notes

This project is intended as a learning-focused operating system. It is not a full modern OS and does not yet include advanced features such as multitasking, user-space security, or a complete Unix-like environment. Instead, it focuses on demonstrating the fundamentals of kernel initialization and system design.

## License

This project is provided for educational purposes. Please review the repository for any licensing details before reuse or redistribution.

## Summary

NovaOS is a compact x86 operating system project that demonstrates how a kernel starts, initializes core subsystems, interacts with hardware, and exposes a minimal shell. It is a strong foundation for learning low-level OS development and system programming.
