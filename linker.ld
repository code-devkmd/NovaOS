ENTRY(_start)

SECTIONS
{
    /* Multiboot v1 requires the header to be in the first 8 KiB of the file. */
    .multiboot ALIGN(4) :
    {
        KEEP(*(.multiboot))
    }

    /* Load the actual kernel at the conventional 1 MiB address. */
    . = 1M;

    .text ALIGN(4) :
    {
        *(.text)
        *(.text.*)
    }

    .rodata ALIGN(4) :
    {
        *(.rodata)
        *(.rodata.*)
    }

    .data ALIGN(4) :
    {
        *(.data)
        *(.data.*)
    }

    .bss ALIGN(4) :
    {
        *(COMMON)
        *(.bss)
        *(.bss.*)
        kernel_end = .;
    }

    /DISCARD/ :
    {
        *(.comment)
        *(.eh_frame)
        *(.note*)
    }
}
