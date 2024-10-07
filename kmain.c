#include "lib.h"
#include "gdt.h"
#include "video.h"
#include "printk.h"
#include "multiboot.h"
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

void kmain()
{
	// print_addr((unsigned char *)0x800, LINES); // GDT
	printk("mmap_length=%d mmap_addr=%x %d",
			mboot_info->mmap_length,
			mboot_info->mmap_addr,
			sizeof(struct mmap_entry));

	int i = 0;
	while (i < mboot_info->mmap_length)
	{
		struct mmap_entry *entry = (struct mmap_entry *)
			(mboot_info->mmap_addr + i);
		if (entry->type == 1 /* unused */)
		{
			printk("base_addr=%lx length=%lx type=%d",
					entry->base_addr,
					entry->length,
					entry->type);
		}
		i += sizeof(struct mmap_entry);
	}
}
