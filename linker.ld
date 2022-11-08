ENTRY(start)

SECTIONS
{
    . = 1M; /* start 1 megabyte in */

    .multiboot BLOCK(4k) : ALIGN(4K)
    {
        KEEP( *(.multiboot)) /* the multiboot header for grub */
    }

	.text BLOCK(4K) : ALIGN(4K)
	{
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