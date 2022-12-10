ENTRY(start)

SECTIONS
{
    . = 0xC0100000; /* start 1 megabyte in */

    _kernel_start = .;
    .multiboot ALIGN(4K) : AT(ADDR(.multiboot) - 0xC0000000)
    {
        KEEP(*(.multiboot)) /* the multiboot header for grub */
    }

	.text ALIGN(4K) : AT(ADDR(.text) - 0xC0000000)
	{
		*(.text) /* text section */
	}

    .rodata ALIGN(4K) : AT(ADDR(.rodata) - 0xC0000000)
    {
        *(.rodata)
    }

    .data ALIGN(4K) : AT(ADDR(.data) - 0xC0000000)
    {
        *(.data)
    }

    .bss ALIGN(4K) : AT(ADDR(.bss) - 0xC0000000)
    {
        *(COMMON)
        *(.bss)
    }

    _kernel_end = .;
}