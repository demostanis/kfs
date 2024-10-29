virt = 0xc0000000;

/* we have to specify it now, since
 * the linker just picks whatever function
 * comes first in .text */
ENTRY(_start) /* in .multiboot.text */

SECTIONS
{
	.multiboot : { KEEP(*(.multiboot)) }

	. = 1M; /* multiboot spec requires .text to be at 0x100000 */
	/* sets up virtual memory and jumps to actual code in .text */ 
	.multiboot.text ALIGN(CONSTANT(MAXPAGESIZE)) : {
		KEEP(*(.multiboot.text))
	}
	.multiboot.bss ALIGN(CONSTANT(MAXPAGESIZE)) : {
		page_directory = .;
		. += 0x1000;
		page_table0 = .;
		. += 0x1000;
	}

	. += virt;
	kernel_begin = . - virt;

	.text ALIGN(CONSTANT(MAXPAGESIZE)) :
		AT(ADDR(.text) - virt) { KEEP(*(.text)) }
	.data ALIGN(CONSTANT(MAXPAGESIZE)) :
		AT(ADDR(.data) - virt) { KEEP(*(.data)) }
	.bss ALIGN(CONSTANT(MAXPAGESIZE)) :
		AT(ADDR(.bss) - virt) { KEEP(*(.bss)) }

	/* was mandated by KFS-2 */
	/*. = 0x800;*/
	.gdt : AT(ADDR(.gdt) - virt) { *(.gdt) }

	/* reserve some room for symtab and strtab */
	. += 0x10000;

	kernel_end = . - virt;
}
