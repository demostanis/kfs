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

struct multiboot_info *multiboot_info;

void kcommon(int magic, struct multiboot_info *info)
{
	multiboot_info = info;

	if (magic != MULTIBOOT_BOOTLOADER_MAGIC)
		shutdown();

	gdt_install();
	clear_video();
	disable_cursor();
}

void kmain()
{
	// print_addr((unsigned char *)0x800, LINES); // GDT
	printk("mem_lower=%x mem_upper=%x",
			multiboot_info->mem_lower,
			multiboot_info->mem_upper);
}
