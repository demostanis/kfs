#include "gdt.h"
#include "video.h"

void print_addr(unsigned char *addr, int lines)
{
	while (lines-- >= 0)
	{
		printk("%x: %x %x %x %x", addr,
				((unsigned char *)addr)[3],
				((unsigned char *)addr)[2],
				((unsigned char *)addr)[1],
				((unsigned char *)addr)[0]
				);
		addr += 4;
	}
}

void kmain()
{
	gdt_install();

	__attribute__((unused)) char *s = "my address is in the stack";
	__attribute__((unused)) int i = 42;
	__attribute__((unused)) int d = 0xdeadbeef;

	disable_cursor();

	print_addr((unsigned char *)&d, 24);
}
