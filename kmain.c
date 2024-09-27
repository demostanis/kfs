#include "gdt.h"
#include "printk.h"

void print_addr(unsigned char *addr, int lines)
{
	while (lines-- >= 0)
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

void kcommon()
{
	gdt_install();
	disable_cursor();
}

void kmain()
{
	print_addr((unsigned char *)0x800, 24); // GDT
}
