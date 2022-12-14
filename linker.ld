ENTRY(start)

SECTIONS
{
    . = 1M; /* start 1 megabyte in */

    .multiboot : ALIGN(4K)
    {
        KEEP(*(.multiboot)) /* the multiboot header for grub */
    }

	.text : ALIGN(4K)
	{
		*(.text) /* text section */
	}

    .rodata : ALIGN(4K)
    {
        *(.rodata)
    }

    .data : ALIGN(4K)
    {
        *(.data)
    }

    .bss : ALIGN(4K)
    {
        *(COMMON)
        *(.bss)
    }
}