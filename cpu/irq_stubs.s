.section .text

.extern irq_handler

.global irq0
.global irq1
.global irq2
.global irq3
.global irq4
.global irq5
.global irq6
.global irq7
.global irq8
.global irq9
.global irq10
.global irq11
.global irq12
.global irq13
.global irq14
.global irq15


.macro IRQ number

irq\number:
    push $\number
    jmp irq_common

.endm


irq_common:
    pusha

    /*
     * Stack after pusha:
     *
     * 32(%esp) = IRQ number
     */

    mov 32(%esp), %eax
    push %eax

    call irq_handler

    add $4, %esp

    popa

    add $4, %esp

    iret


IRQ 0
IRQ 1
IRQ 2
IRQ 3
IRQ 4
IRQ 5
IRQ 6
IRQ 7
IRQ 8
IRQ 9
IRQ 10
IRQ 11
IRQ 12
IRQ 13
IRQ 14
IRQ 15
