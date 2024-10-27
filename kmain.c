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

	void *ptr = kmalloc(4054);
	printk("ptr1 = %p", ptr);
	ptr = kmalloc(1);
	printk("ptr2 = %p", ptr);

	ptr = kmalloc(15);
	printk("ptr3 = %p", ptr);
	ptr = kmalloc(15);
	printk("ptr4 = %p", ptr);
	ptr = kmalloc(15);
	printk("ptr5 = %p", ptr);

	ptr = kmalloc(10000);
	memset(ptr, 'A', 10000);
	printk("ptr6 = %p", ptr);

	ptr = kmalloc(10);
	printk("ptr7 = %p", ptr);
	kfree(ptr);
	printk("kfree(ptr7)");
	ptr = kmalloc(10);
	printk("ptr8 = %p", ptr);
}
