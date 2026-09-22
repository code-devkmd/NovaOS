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

# NovaOS — Project Context & Development History

## Overview

**NovaOS** is a hobby operating system being developed from scratch primarily in **C and x86 Assembly**.

The main goal is not to clone Linux or Windows, but to understand how an operating system works by building the major components ourselves and eventually turning NovaOS into a **usable, independent desktop operating system**.

Target architecture:

* 32-bit x86
* GRUB / Multiboot booting
* C + Assembly kernel
* Custom kernel subsystems
* Custom filesystem
* Custom shell
* Eventually a graphical desktop environment

The long-term vision is to move from a simple educational kernel into something that can actually be used for basic everyday tasks.

---

# Development Progress

## v0.6 — Stable Kernel Foundation

Implemented the basic kernel foundation:

* GRUB booting
* Multiboot support
* Kernel entry point
* GDT
* IDT
* Interrupt infrastructure
* Serial logging
* Kernel panic functionality
* Basic IRQ handling
* PS/2 keyboard interrupts

This established the basic environment for developing the rest of NovaOS.

---

## v0.7 — PIT Timer

Added the Programmable Interval Timer.

NovaOS gained:

* PIT initialization
* Timer interrupts
* Kernel tick counter
* Basic timing infrastructure

The timer later became useful for other kernel features.

---

## v0.8 — Physical Memory Manager

Implemented basic physical memory management.

NovaOS can now keep track of physical memory pages and allocate/free physical memory.

This created the foundation for:

* Paging
* Kernel heap
* Process memory
* User-space memory
* More advanced memory management

---

## v0.9 — Paging

Implemented basic paging.

NovaOS now has the foundation for virtual memory.

This is an important step toward eventually supporting:

* User mode
* Processes
* Memory isolation
* Dynamic memory
* Executable programs

---

## v1.0 — Kernel Heap

Added a kernel heap for dynamic memory allocation.

Kernel components can now request memory dynamically rather than relying entirely on static/global memory.

This became important for filesystem structures, drivers, shell data, and future processes.

---

# Filesystem Development

## v1.1 — VFS

Implemented a **Virtual File System** layer.

The VFS provides an abstraction between the kernel and actual filesystem implementations.

This allows NovaOS to eventually support different storage backends without rewriting the entire shell or kernel.

---

## v1.2 — RAMFS

Added a RAM-based filesystem.

NovaOS can maintain files and directories in memory.

This provided the basis for developing file operations before persistent disk storage was ready.

---

## v1.3 — File Commands

Added basic shell filesystem commands:

```text
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

NovaOS began behaving more like an actual command-line operating system.

---

## v1.4 — ATA PIO

Added ATA PIO storage support.

This allowed NovaOS to communicate with an IDE-style storage device.

The development environment was designed around:

```text
CD/DVD
   ↓
NovaOS.iso
   ↓
GRUB
   ↓
NovaOS kernel

Hard Disk
   ↓
novaos-disk.img
   ↓
NovaOS storage
```

The ISO boots the operating system while the disk image can be used for persistent storage.

---

## v1.5 — NovaFS

Started developing a persistent NovaOS filesystem.

The objective is to move beyond a temporary RAM filesystem and allow files to survive rebooting.

---

## v1.6 — File Operations

Expanded persistent file handling.

The filesystem is becoming a proper foundation for future applications and the graphical file manager.

---

# Shell Development

## v1.7 — Improved Shell

The shell became more Linux-like while remaining intentionally small.

The project philosophy is to avoid adding dozens of unnecessary commands.

Current useful commands include:

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

Several diagnostic/experimental commands were deliberately removed because they were not useful to the normal user.

---

## v1.8 — Terminal Editing

The shell received much more practical terminal interaction.

Implemented:

* Blinking cursor
* Cursor movement
* Left/right arrow keys
* Home
* End
* Backspace
* Delete
* Command history
* Up/down history navigation
* Ctrl+C
* Tab completion
* Improved keyboard handling

At this point, the terminal started feeling like an actual shell instead of a basic command interpreter.

---

# Graphics Development

## v1.9 — Framebuffer

Started the transition from VGA text mode toward graphical output.

Implemented:

* Multiboot framebuffer parsing
* Framebuffer abstraction
* Graphics API
* 8×8 bitmap font
* Framebuffer terminal
* VGA fallback
* Basic drawing primitives
* Basic colors

The project also encountered an important boot problem involving the Multiboot graphics configuration.

The problem was traced to:

1. An incorrect Multiboot header layout.
2. The Multiboot section being outside the first 8 KiB.

The boot code and linker layout were corrected.

However, forcing the graphical framebuffer mode caused a black screen on the test environment, so the stable versions continued using VGA/text mode while graphics development continued separately.

---

# v2.0 — PS/2 Mouse

Added PS/2 mouse support.

Implemented:

* PS/2 controller communication
* Mouse initialization
* Mouse streaming
* Mouse packet parsing
* Mouse movement
* Mouse buttons
* IRQ12
* Mouse coordinate tracking
* Boundary clamping

The interrupt system was updated to handle both:

```text
IRQ1  → Keyboard
IRQ12 → Mouse
```

A significant keyboard problem appeared after adding the mouse because both devices share the PS/2 controller.

The keyboard driver was subsequently improved with proper controller initialization and synchronization.

---

# v2.0.1

Improved PS/2 controller initialization.

Added handling for:

* Keyboard controller configuration
* Keyboard port
* Mouse port
* IRQ1
* IRQ12
* Controller state

However, keyboard input still had problems in testing.

---

# v2.0.2 — Working Keyboard + Mouse Baseline

The PS/2 implementation was strengthened.

The initialization now:

* Disables both PS/2 ports
* Flushes stale controller data
* Configures keyboard IRQ
* Configures keyboard clock
* Re-enables the keyboard port
* Resets the keyboard
* Explicitly enables keyboard scanning
* Clears leftover responses
* Initializes the mouse separately

This version was tested successfully.

**v2.0.2 is therefore the known working keyboard + mouse baseline.**

---

# v2.1 — Graphical Mouse Cursor

The next objective was to combine the framebuffer system with the PS/2 mouse.

Planned functionality:

```text
Framebuffer
     +
PS/2 Mouse
     ↓
Graphical mouse pointer
```

The intended system includes:

* Screen coordinates
* Graphical cursor
* Mouse movement
* Cursor boundaries
* Cursor redraw
* Terminal preservation
* VGA fallback

The first v2.1 build resulted in a black screen during testing.

A v2.1.1 build was created with additional graphics/GRUB configuration changes, including:

```text
gfxmode=1024x768x32
gfxpayload=1024x768x32
terminal_output gfxterm
```

---

# Current Architecture

The project currently has the beginnings of a real operating-system architecture:

```text
                    NovaOS
                       │
        ┌──────────────┴──────────────┐
        │                             │
      Kernel                       User Interface
        │                             │
 ┌──────┼───────────┐          ┌──────┴──────┐
 │      │           │          │             │
CPU   Memory      Drivers    Shell        Graphics
 │      │           │          │             │
GDT   PMM         Keyboard   Commands     Framebuffer
IDT   Paging       Mouse     Filesystem     Cursor
IRQ   Heap         Timer
```

Filesystem:

```text
Shell
  ↓
VFS
  ↓
RAMFS / NovaFS
  ↓
Storage layer
  ↓
ATA PIO
  ↓
Disk
```

This separation will make future development much easier.

---

# Current Shell

NovaOS intentionally keeps its shell relatively small.

Current commands:

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

The philosophy is:

> Every command should have a useful purpose.

More commands can be added later when they are actually needed.

---

# Future Plan — From Kernel to Usable OS

The next phase should focus on turning NovaOS from a kernel project into an actual operating system.

## v2.1 — Stable Graphics

First, make graphics reliable.

Goals:

* Reliable framebuffer detection
* Correct framebuffer memory handling
* Graphics initialization
* Screen clearing
* Pixel drawing
* Rectangle drawing
* Text rendering
* VGA fallback
* Proper framebuffer memory reservation

An important issue to solve is preventing the physical memory manager from allocating the framebuffer's physical memory to something else.

---

# v2.2 — Window System

After graphics are stable:

```text
Desktop
 ├── Window
 ├── Window
 └── Window
```

Implement:

* Window structure
* Window positions
* Window sizes
* Window borders
* Window title bars
* Moving windows
* Closing windows
* Active window
* Window redraw

The goal is not to immediately create a complicated desktop environment.

Start with a simple window manager.

---

# v2.3 — GUI Toolkit

Create reusable GUI components:

* Buttons
* Labels
* Text boxes
* Panels
* Checkboxes
* Menus
* Scroll areas
* Basic icons

This allows applications to share the same UI system.

---

# v2.4 — GUI Terminal

Turn the existing terminal into a graphical application.

Example:

```text
┌──────────────────────────────────────────┐
│ Nova Terminal                         × │
├──────────────────────────────────────────┤
│ NovaOS> ls                               │
│ Documents   Downloads   test.txt         │
│ NovaOS>                                   │
│                                          │
└──────────────────────────────────────────┘
```

The existing shell will become the backend while the GUI provides the interface.

---

# v2.5 — File Manager

Create a graphical file manager.

Example:

```text
Nova Files

Home
├── Documents
├── Downloads
├── Pictures
├── Projects
└── test.txt
```

Features:

* Browse directories
* Open files
* Create folders
* Rename
* Delete
* Copy
* Move
* Basic file properties

This will make NovaOS much more usable.

---

# v2.6 — Input System

Create a unified input layer.

Instead of every application directly talking to the keyboard/mouse drivers:

```text
Keyboard Driver ─┐
                 ├── Input System ── Applications
Mouse Driver ────┘
```

This will make GUI applications easier to develop.

---

# v2.7 — Audio

Eventually add basic sound support.

Possible goals:

* PC speaker
* Basic audio device support
* System notification sounds
* Simple audio playback

Audio should come after the core GUI and application architecture are stable.

---

# v2.8 — Networking

Add networking support.

Development could progress roughly as:

```text
Network Driver
      ↓
Ethernet
      ↓
ARP
      ↓
IPv4
      ↓
ICMP
      ↓
UDP
      ↓
TCP
      ↓
Sockets
```

Eventually this could allow:

* Network configuration
* Ping
* Local network communication
* TCP/UDP applications
* HTTP client
* Internet-enabled applications

---

# v2.9 — USB

Eventually support common USB devices.

Potential targets:

* USB keyboard
* USB mouse
* USB storage

USB is significantly more complicated than PS/2, so it should come after the existing architecture is mature.

---

# v3.0 — Nova Desktop

The first major desktop release.

Possible design:

```text
┌──────────────────────────────────────────────┐
│ NovaOS                         21:45          │
├──────────────────────────────────────────────┤
│                                              │
│       Desktop                                │
│                                              │
│       [Files]     [Terminal]     [Settings]  │
│                                              │
│                                              │
├──────────────────────────────────────────────┤
│  Nova Menu                         Network   │
└──────────────────────────────────────────────┘
```

Components:

* Desktop
* Taskbar/panel
* Application launcher
* File manager
* Terminal
* Settings
* Window manager
* Mouse cursor
* Basic icons
* Notifications

This would be the point where NovaOS starts looking and behaving like a conventional desktop OS.

---

# Kernel Evolution

While the GUI is being developed, the kernel should eventually gain proper process support.

## User Mode

Move applications out of kernel space.

Architecture:

```text
Kernel Space
     │
     ├── Drivers
     ├── Memory
     ├── Filesystem
     └── Scheduler
           │
       System Calls
           │
User Space
     │
     ├── Terminal
     ├── File Manager
     ├── Settings
     └── Applications
```

This is essential for making NovaOS a real multi-application operating system.

---

# Processes

Implement:

* Process structure
* Process creation
* Process termination
* Process IDs
* Address spaces
* Context switching
* Process states

Eventually:

```text
Process 1 → Terminal
Process 2 → File Manager
Process 3 → Settings
Process 4 → User Application
```

---

# Scheduler

The PIT timer already provides a foundation for scheduling.

Eventually:

```text
Timer IRQ
    ↓
Scheduler
    ↓
Process A
    ↓
Process B
    ↓
Process C
    ↓
Process A
```

Start with a simple scheduler before implementing more advanced scheduling algorithms.

---

# System Calls

Create a stable interface between applications and the kernel.

For example:

```text
open()
read()
write()
close()
malloc()
exit()
fork()/spawn()
```

Applications should not directly access hardware.

Instead:

```text
Application
     ↓
System Call
     ↓
Kernel
     ↓
Driver
     ↓
Hardware
```

---

# Executable Programs

Add an executable format such as ELF.

Eventually NovaOS should be able to do something like:

```text
NovaOS> run hello
Hello from user space!
```

Rather than having everything compiled directly into the kernel.

---

# NovaOS Standard Library

Create a small user-space library.

Potential components:

```text
libnova
├── stdio
├── stdlib
├── string
├── memory
├── file
├── process
└── graphics
```

This will make writing NovaOS applications much easier.

---

# Applications

Once user mode, system calls, executable loading and the GUI are available, NovaOS can start having actual applications.

Initial applications could include:

```text
Terminal
File Manager
Text Editor
Calculator
Settings
System Monitor
Image Viewer
```

Later:

```text
Web Browser
Music Player
Code Editor
PDF Viewer
```

These should be developed after the operating-system foundations are stable.

---

# Long-Term Storage System

NovaFS can eventually become a proper filesystem.

Potential features:

* File permissions
* File metadata
* Timestamps
* Larger files
* Journaling
* Better allocation
* Directory indexing
* Filesystem checking
* Disk recovery

The objective is to make persistent storage reliable enough for normal use.

---

# Security

As NovaOS becomes usable, security becomes important.

Future work:

* User accounts
* User/kernel separation
* File permissions
* Process isolation
* Memory protection
* Safe system calls
* Application sandboxing

Security should be designed into the architecture rather than added only at the end.

---

# Eventually: A Complete NovaOS Stack

The long-term architecture could look like:

```text
┌─────────────────────────────────────────────┐
│                 Applications                │
│                                             │
│ Browser │ Editor │ Files │ Terminal │ Apps │
├─────────────────────────────────────────────┤
│                  GUI                        │
│ Window Manager │ Widgets │ Desktop │ Input  │
├─────────────────────────────────────────────┤
│             User-Space Libraries            │
├─────────────────────────────────────────────┤
│                System Calls                 │
├─────────────────────────────────────────────┤
│                  Kernel                     │
│                                             │
│ Scheduler │ Processes │ Memory │ VFS        │
│ IPC       │ Security  │ Network │ Drivers  │
├─────────────────────────────────────────────┤
│                 Hardware                    │
│ CPU │ RAM │ Disk │ USB │ GPU │ Network │etc│
└─────────────────────────────────────────────┘
```

---

# Ultimate Goal

NovaOS should gradually evolve through these stages:

```text
Educational Kernel
        ↓
Command-Line OS
        ↓
Storage-Capable OS
        ↓
Graphical OS
        ↓
Multi-Process OS
        ↓
Application Platform
        ↓
Usable Desktop OS
```

The key principle is to **build the foundations first and add usability on top of them**, rather than trying to build a desktop immediately.

The current known stable milestone is **v2.0.2 (keyboard + mouse)**, while **v2.1.x graphics/mouse-cursor development is ongoing**. The long-term target is **NovaOS 3.x as a genuinely usable desktop operating system**, with later versions focused on applications, networking, security, hardware support, and reliability.
