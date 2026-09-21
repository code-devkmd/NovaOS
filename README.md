# NovaOS

<<<<<<< HEAD
**NovaOS** is a small hobby operating system I'm building from scratch to learn how operating systems work at a lower level.

This project started as an experiment to understand what happens underneath the programs we normally run on a computer — from booting a machine to handling keyboard input and displaying text on the screen.

> This is a learning project and is still very much a work in progress.

## Current Features

* 32-bit x86 kernel
* Custom boot code
* Basic terminal output
* Screen clearing and text colors
* Keyboard input
* Basic backspace handling
* Simple text input
* GRUB bootable ISO
* QEMU support for testing

The current kernel starts with a simple NovaOS welcome screen and allows you to type text and have the system display what you entered.

## Project Structure
=======
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
>>>>>>> master

```text
NovaOS/
├── boot/
<<<<<<< HEAD
│   └── boot.s
│
├── kernel/
│   ├── kernel.c
│   ├── terminal.c
│   ├── terminal.h
│   ├── io.c
│   ├── io.h
│   ├── keyboard.c
│   └── keyboard.h
│
├── iso/
│   └── boot/
│       └── grub/
│
├── linker.ld
├── Makefile
└── README.md
```

## How It Works

NovaOS is built as a **freestanding 32-bit x86 system**, meaning it doesn't depend on an existing operating-system environment or standard C library.

The basic flow is:

```text
Computer starts
      ↓
Boot code
      ↓
Kernel
      ↓
Terminal initialization
      ↓
Keyboard input
      ↓
User interaction
```

The kernel is written in C, with assembly used for the low-level boot process.

## Building

You'll need tools such as:

* GCC
* GNU Make
* GNU Binutils
* GRUB tools
* QEMU

Build the ISO with:
=======
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
>>>>>>> master

```bash
make
```

<<<<<<< HEAD
This produces:

```text
NovaOS.iso
```

## Running

You can run NovaOS using QEMU:
=======
Run in QEMU:
>>>>>>> master

```bash
make run
```

<<<<<<< HEAD
To clean the generated files:
=======
Clean generated files:
>>>>>>> master

```bash
make clean
```

<<<<<<< HEAD
## Example

When NovaOS starts, it displays something similar to:

```text
========================================
            NovaOS v0.4
========================================

Welcome to NovaOS!
NovaOS>
```

You can type text using the keyboard, and NovaOS will display it back.

## Why I Made This

I've worked with higher-level programming for a while, but I wanted to understand computers from a different perspective.

Instead of building another application, I decided to build something much closer to the hardware.

Through NovaOS, I'm learning about:

* How a computer boots
* CPU architecture
* Memory and addresses
* Assembly
* C at a low level
* Kernel development
* Hardware I/O
* Interrupts
* Filesystems
* Operating-system design

## Roadmap

There is still a lot I want to explore.

* [ ] Better keyboard handling
* [ ] Interrupt handling
* [ ] Memory management
* [ ] Basic shell
* [ ] Timer support
* [ ] Multitasking
* [ ] Filesystem
* [ ] User programs
* [ ] More hardware support

The roadmap will probably change as I learn more.

## Status

**Early development / hobby project**

NovaOS is not intended to replace an existing operating system. It's mainly a project for learning, experimenting, breaking things, and understanding how operating systems work underneath the abstractions.

## Tech

* **C**
* **x86 Assembly**
* **GNU Make**
* **GRUB**
* **QEMU**

---

Built as a learning project by **Code-Dev**.
=======
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
>>>>>>> master
