ENTRY(start)

SECTIONS
{
    . = 1M; /* start 1 megabyte in */

    _kernel_start = .;
    _kernel_voffset = 0xC0000000;

    .multiboot_header :
    {
        *(.multiboot_header) /* the multiboot header for grub */
    }
    .multiboot.text :
    {
        *(.multiboot.text) /* the multiboot header for grub */
    }

    . += _kernel_voffset;
	.text ALIGN(4K) : AT(ADDR(.text) - _kernel_voffset)
	{
		*(.text) /* text section */
	}

    .rodata ALIGN(4K) : AT(ADDR(.rodata) - _kernel_voffset)
    {
        *(.rodata)
    }

    .data ALIGN(4K) : AT(ADDR(.data) - _kernel_voffset)
    {
        *(.data)
    }

    .bss ALIGN(4K) : AT(ADDR(.bss) - _kernel_voffset)
    {
        *(.bss)
    }
    _kernel_end = .;
}