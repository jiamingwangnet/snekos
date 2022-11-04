ENTRY(start)

SECTIONS
{
    . = 1M; /* start 1 megabyte in */

	.text BLOCK(4K) : ALIGN(4K)
	{
        *(.multiboot) /* the multiboot header for grub */
		*(.text) /* text section */
	}

    .rodata BLOCK(4K) : ALIGN(4K)
    {
        *(.rodata)
    }

    .bss BLOCK(4K) : ALIGN(4K)
    {
        *(COMMON)
        *(.bss)
    }
}