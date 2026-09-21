.section .text

.extern isr_handler

.global isr0
.global isr1
.global isr2
.global isr3
.global isr4
.global isr5
.global isr6
.global isr7
.global isr8
.global isr9
.global isr10
.global isr11
.global isr12
.global isr13
.global isr14
.global isr15
.global isr16
.global isr17
.global isr18
.global isr19
.global isr20
.global isr21


.macro ISR_NO_ERROR number
isr\number:
    push $0
    push $\number
    jmp isr_common
.endm


.macro ISR_ERROR number
isr\number:
    push $\number
    jmp isr_common
.endm


isr_common:
    pusha

    /*
     * At this point the stack contains:
     *
     * registers
     * interrupt number
     * error code
     */
    mov 36(%esp), %eax
    push %eax
    mov 32(%esp), %eax
    push %eax

    call isr_handler

    add $8, %esp
    popa

    add $8, %esp

    iret


ISR_NO_ERROR 0
ISR_NO_ERROR 1
ISR_NO_ERROR 2
ISR_NO_ERROR 3
ISR_NO_ERROR 4
ISR_NO_ERROR 5
ISR_NO_ERROR 6
ISR_NO_ERROR 7

ISR_ERROR 8

ISR_NO_ERROR 9
ISR_ERROR 10
ISR_ERROR 11
ISR_ERROR 12
ISR_ERROR 13
ISR_ERROR 14

ISR_NO_ERROR 15
ISR_NO_ERROR 16
ISR_ERROR 17
ISR_NO_ERROR 18
ISR_NO_ERROR 19
ISR_NO_ERROR 20
ISR_ERROR 21
