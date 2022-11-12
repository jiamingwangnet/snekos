ENTRY(start)

SECTIONS
{
    . = 1M; /* start 1 megabyte in */

    .multiboot :
    {
        KEEP(*(.multiboot)) /* the multiboot header for grub */
    }

	.text :
	{
		*(.text) /* text section */
	}

    .rodata ALIGN(4K) :
    {
        *(.rodata)
    }

    .data ALIGN(4K) :
    {
        *(.data)
    }

    .bss ALIGN(4K) :
    {
        *(COMMON)
        *(.bss)
    }
}