#include "lib.h"
#include "gdt.h"
#include "idt.h"
#include "video.h"
#include "printk.h"
#include "multiboot.h"
#include "pmem.h"
#include "vmem.h"
#include "kmalloc.h"
#include "keyboard.h"
#include "tests.h"

void print_addr(unsigned char *addr, int lines)
{
	while (lines-- > 0)
	{
		printk("%8x: %2x %2x %2x %2x |%4B|", addr,
				((unsigned char *)addr)[3],
				((unsigned char *)addr)[2],
				((unsigned char *)addr)[1],
				((unsigned char *)addr)[0],
				addr
				);
		addr += 4;
	}
}

struct multiboot_info *mboot_info;

void kcommon(int magic, struct multiboot_info *info)
{
	if (magic != MULTIBOOT_BOOTLOADER_MAGIC)
		panic("kernel needs to be booted from multiboot1");

	if (info->flags & MULTIBOOT_INFO_MEM_MAP == 0)
		panic("missing memory map");

	if (info->flags & MULTIBOOT_INFO_ELF_SHDR == 0)
		panic("missing ELF sections");

	mboot_info = skip_bytes(info, UNUSED_FIELDS_PADDING);

	init_pmem_regions();
	init_page_directory();

	gdt_install();
	idt_install();
	clear_video();
	disable_cursor();

	// TODO: why does it say page already mapped?
	unmap_page((addr)reloc(mboot_info));
	map_phys_to_virt((addr)mboot_info, (addr)reloc(mboot_info));
	load_symbols(reloc(mboot_info));
}

char prompt[] = "kfs # ";
int user_input_start = sizeof(prompt)-1;

void kmain()
{
	while (1)
	{
		putstr(prompt);
		char *line = get_line();
		putstr(line);
		if (*line)
			putchar('\n');

		if (strcmp(line, "panic") == 0)
			panic("triggered panic");
		else if (strcmp(line, "fault") == 0)
			*((volatile char *)0x0) = 0;

		kfree(line);
	}
}
