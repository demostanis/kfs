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

void kmain()
{
	gdt_install();

	disable_cursor();

	__attribute__((unused)) char *s = "my address is in the stack";
	__attribute__((unused)) int i = 42;
	__attribute__((unused)) int d = 0xdeadbeef;

	print_addr((unsigned char *)s, 24);
}
