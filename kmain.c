#include "lib.h"
#include "gdt.h"
#include "video.h"
#include "printk.h"
#include "multiboot.h"
#include "pmem.h"
#include "vmem.h"
#include "kmalloc.h"
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

	mboot_info = skip_bytes(info, UNUSED_FIELDS_PADDING);

	init_pmem_regions();
	init_page_directory();

	gdt_install();
	clear_video();
	disable_cursor();
}

void kmain()
{
	printk("Hello, world!");

	void *ptr = kmalloc(10);
	while (ptr)
	{
		printk("ptr = %p", ptr);
		ptr = kmalloc(10);
	}
}
