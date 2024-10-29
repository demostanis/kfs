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

#define handle_interrupt(i) __handle_interrupt(i)
#define __handle_interrupt(i) \
	void isr_wrapper_##i(); \
	idt_set_descriptor(i, isr_wrapper_##i);

#define KEYBOARD_IV 9
#define PAGE_FAULT_IV 14

void idt_install()
{
	idtp.addr = (addr)idt;
	idtp.limit = sizeof(idt);

	handle_interrupt(KEYBOARD_IV);
	handle_interrupt(PAGE_FAULT_IV);

	outb(PIC1_DATA, 0xfd);
	outb(PIC2_DATA, 0xff);

	load_idt();
}
