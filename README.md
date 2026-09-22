# NovaOS

NovaOS is a small hobby operating system written mainly in **C and x86 Assembly**.

The goal is to learn how an operating system works by building its parts from the ground up.

## Current Version

**v2.1.1 — Graphics / Mouse Cursor**

This version is working toward graphical mouse support and framebuffer graphics.

> Note: The graphical mode is still being developed. If you get a black screen, use the VGA/text-mode version while graphics are being fixed.

## What NovaOS Can Do

- 32-bit x86 kernel
- GRUB booting
- VGA text terminal
- Keyboard input
- Command-line shell
- Command history and editing
- PS/2 mouse driver
- Mouse interrupts
- PIT timer
- GDT and IDT
- IRQ handling
- Physical memory management
- Paging
- Kernel heap
- Virtual filesystem
- RAM filesystem
- Basic file operations
- ATA PIO storage support
- Framebuffer/graphics support under development

## Shell Commands

```text
help
clear
echo
ls
cd
pwd
mkdir
touch
write
cat
rm
cp
mv
```

Type `help` inside NovaOS to see the available commands.

## Project Structure

```text
NovaOS/
├── boot/       Boot code
├── cpu/        GDT, IDT, IRQ and interrupt code
├── include/    Header files
├── kernel/     Main kernel and drivers
├── linker.ld   Kernel memory layout
├── Makefile    Build instructions
└── README.md   This file
```

## Requirements

On Ubuntu, install the basic build and testing tools:

```bash
sudo apt update
sudo apt install build-essential grub-pc-bin grub-common xorriso qemu-system-x86
```

## Build and Run

Enter the project directory:

```bash
cd NovaOS
```

Clean old files:

```bash
make clean
```

Build the kernel and ISO:

```bash
make run
```

This creates:

```text
NovaOS.iso
```

The ISO is the bootable part. 

## Roadmap

```text
v2.1   Graphical mouse cursor
v2.2   Window system
v2.3   GUI widgets
v2.4   GUI terminal
v2.5   File manager
v3.0   Simple desktop
```

Later:

- User mode
- System calls
- Processes
- Scheduler
- ELF programs
- Small C library

## Project Philosophy

NovaOS is intentionally small and understandable.

It is not meant to copy Linux.

The goal is to understand operating systems by building one piece at a time.

---
