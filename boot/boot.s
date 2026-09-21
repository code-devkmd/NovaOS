.set ALIGN,    1<<0
.set MEMINFO,  1<<1
.set FLAGS,    ALIGN | MEMINFO
.set MAGIC,    0x1BADB002
.set CHECKSUM, -(MAGIC + FLAGS)

.section .multiboot,"a"
.align 4

.long MAGIC
.long FLAGS
.long CHECKSUM


.section .bss
.align 16

stack_bottom:
.skip 16384

stack_top:


.section .text
.global _start
.extern kernel_main

_start:
    /*
     * Set up NovaOS's own 16 KiB kernel stack.
     *
     * The x86 stack grows downward, so ESP starts
     * at stack_top.
     */
    mov $stack_top, %esp

    /*
     * Keep the stack aligned for C code.
     */
    and $-16, %esp

    push %ebx
    push %eax
    call kernel_main


hang:
    cli
    hlt
    jmp hang
