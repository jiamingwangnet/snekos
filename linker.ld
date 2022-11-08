ENTRY(start)

SECTIONS
{
    . = 1M; /* start 1 megabyte in */

    _kernel_start = .;

    .multiboot BLOCK(4k) : ALIGN(4K)
    {
        KEEP( *(.multiboot)) /* the multiboot header for grub */
    }

    .multiboot.text :
    {
        *(.multiboot.text)
    }

    . += 0xC0000000;

	.text ALIGN(4K) : AT (ADDR (.text) - 0xC0000000)
	{
		*(.text) /* text section */
	}

    .rodata ALIGN(4K) : AT (ADDR (.rodata) - 0xC0000000)
    {
        *(.rodata)
    }

    .bss ALIGN(4K) : AT (ADDR (.bss) - 0xC0000000)
    {
        *(COMMON)
        *(.bss)
    }

    _kernel_end = .;
}