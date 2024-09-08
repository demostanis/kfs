SECTIONS
{
	.multiboot  : { KEEP(*(.multiboot)) }
	.bss : { *(.bss) }

	. = 1M; /* multiboot spec requires .text to be at 0x100000 */
	.text : { KEEP(*(.text)) }
}
