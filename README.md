# NovaOS

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

```text
NovaOS/
├── boot/
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

```bash
make
```

This produces:

```text
NovaOS.iso
```

## Running

You can run NovaOS using QEMU:

```bash
make run
```

To clean the generated files:

```bash
make clean
```

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
