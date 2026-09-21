.section .text

.global gdt_flush

gdt_flush:
    /*
     * Reload CS with selector 0x08.
     *
     * A far return changes CS and continues execution
     * at the next instruction.
     */
    push $0x08
    push $reload_segments
    lret

reload_segments:

    /*
     * Reload kernel data segments.
     *
     * GDT entry 2 = 0x10.
     */
    mov $0x10, %ax

    mov %ax, %ds
    mov %ax, %es
    mov %ax, %fs
    mov %ax, %gs
    mov %ax, %ss

    ret
