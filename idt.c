#include "idt.h"

struct idt_entry idt[IDT_NENTRIES];
struct idt_ptr idtp;

void idt_set_descriptor(int n, void *fptr)
{
	idt[n].addr_low = (addr)fptr & 0xffff;
	idt[n].addr_high = (addr)fptr >> 16;
	idt[n].code_segment = 0x8;
	// https://wiki.osdev.org/Interrupt_Descriptor_Table
	// 32-bit interrupt gate
	idt[n].flags = 0b10001110;
}

void idt_install()
{
	idtp.addr = (addr)idt;
	idtp.limit = sizeof(idt);

	int i = 0;
	while (i < IDT_NENTRIES)
		idt_set_descriptor(i++, isr_wrapper);

	outb(PIC1_DATA, 0xfd);
	outb(PIC2_DATA, 0xff);

	load_idt();
}
