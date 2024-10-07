SECTIONS
{
	.multiboot  : { KEEP(*(.multiboot)) }

	. = 1M; /* multiboot spec requires .text to be at 0x100000 */
	.text : { KEEP(*(.text)) }

	/* was mandated by KFS-2 */
	/*. = 0x800;*/
	.gdt : { *(.gdt) }

	kernel_end = .;
}
