ENTRY(start)

SECTIONS
{
    . = 1M; /* start 1 megabyte in */
    kernel_voffset = 0xFFFFFFFF80000000;

    _kernel_start = .;
    .multiboot ALIGN(4K) : AT(ADDR(.multiboot))
    {
        KEEP(*(.multiboot)) /* the multiboot header for grub */
    }

    .multiboot.text ALIGN(4K) : AT(ADDR(.multiboot.text))
    {
        *(.multiboot.text)
    }

    . += kernel_voffset;

	.text ALIGN(4K) : AT(ADDR(.text) - kernel_voffset)
	{
		*(.text) /* text section */
	}

    .rodata ALIGN(4K) : AT(ADDR(.rodata) - kernel_voffset)
    {
        *(.rodata)
    }

    .data ALIGN(4K) : AT(ADDR(.data) - kernel_voffset)
    {
        *(.data)
    }

    .bss ALIGN(4K) : AT(ADDR(.bss) - kernel_voffset)
    {
        *(COMMON)
        *(.bss)
    }
    _kernel_end = .;
}