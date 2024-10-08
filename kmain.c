#include "lib.h"
#include "gdt.h"
#include "video.h"
#include "printk.h"
#include "multiboot.h"
#include "pmem.h"
#include "vmem.h"
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

	gdt_install();
	clear_video();
	disable_cursor();
}

/* populated by the linker */
extern void *kernel_end;
#define kernel_begin 0x100000

void kmain()
{
	// print_addr((unsigned char *)0x800, LINES); // GDT
	printk("mmap_length=%d mmap_addr=%x",
			mboot_info->mmap_length,
			mboot_info->mmap_addr);

	usize i = 0;
	while (i < mboot_info->mmap_length)
	{
		struct mmap_entry *entry = (struct mmap_entry *)
			(mboot_info->mmap_addr + i);
		if (entry->type == 1 /* unused */)
		{
			printk("base_addr=%lx length=%lx",
					entry->base_addr,
					entry->length);
			bitmap_init_region(entry->base_addr, entry->length);
		}
		i += sizeof(struct mmap_entry);
	}

	bitmap_deinit_region(kernel_begin, (u32)kernel_length);
	bitmap_deinit_region(0, 0x1000000);

	pmem_info();

	init_page_directory();
}
