.set ALIGN,    1<<0
.set MEMINFO,  1<<1
.set VIDEO,    1<<2
.set FLAGS,    ALIGN | MEMINFO | VIDEO
.set MAGIC,    0x1BADB002
.set CHECKSUM, -(MAGIC + FLAGS)

.section .multiboot,"a"
.align 4

.long MAGIC
.long FLAGS
.long CHECKSUM

/* Multiboot v1 address fields. NovaOS is loaded normally by GRUB,
 * so these are zero and let GRUB choose the load addresses. */
.long 0              /* header_addr */
.long 0              /* load_addr */
.long 0              /* load_end_addr */
.long 0              /* bss_end_addr */
.long 0              /* entry_addr */

/* Request a linear 32-bit RGB framebuffer from GRUB. */
.long 0              /* mode_type: linear graphics */
.long 1024           /* width */
.long 768            /* height */
.long 32              /* depth */


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
