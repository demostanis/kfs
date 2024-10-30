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

	handle_interrupt(KEYBOARD_IV);
	handle_interrupt(PAGE_FAULT_IV);
	handle_interrupt(GPF_IV);

	outb(PIC1_DATA, 0b11111101); // unmask keyboard (int 1)
	outb(PIC2_DATA, 0b11111111);

	// software interrupts
	// (note: for the signal to be emittable, it needs
	// to be specified in emit_signal() (asmbits.s))
	handle_interrupt(LINE);
	handle_interrupt(SYSCALL);

	load_idt();
}
