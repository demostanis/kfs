#include "gdt.h"

/* null entry, kernel code,
 * kernel data, kernel stack,
 * user code, user data &
 * user stack */
#define nentries 5
struct gdt_entry gdt[nentries] __attribute__((section(".gdt")));
struct gdt_ptr gp;

static void gdt_set_gate(u64 base, u64 limit, u8 access, u8 flags)
{
	static int n = 0;

	gdt[n].base_low = base & 0xFFFF;
	gdt[n].base_middle = (base >> 16) & 0xFF;
	gdt[n].base_high = (base >> 24) & 0xFF;

	gdt[n].limit_low = limit & 0xFFFF;

	gdt[n].granularity = ((limit >> 16) & 0xF) | (flags << 4);
	gdt[n].access = access;

	n++;
}

void gdt_install()
{
	gp.size = sizeof(struct gdt_entry) * nentries - 1;
	gp.addr = (u32)&gdt;

	gdt_set_gate(null);
	gdt_set_gate(kernel_code);
	gdt_set_gate(kernel_data);
	gdt_set_gate(user_code);
	gdt_set_gate(user_data);

	gdt_flush();
}
