ENTRY(start)

SECTIONS
{
    . = 1M; /* start 1 megabyte in */

    .boot :
	{
		KEEP(*(.multiboot)) /* the multiboot header for grub */
	}

	.text :
	{
		*(.text) /* text section */
	}
}