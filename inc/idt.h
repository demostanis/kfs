#pragma once

#include "lib.h"

#define IDT_NENTRIES 256

#define PIC1_DATA 0x21
#define PIC2_DATA 0xa1

// send EOI to PIC
#define ACK() outb(0x20, 0x20);

#define handle_interrupt(i) __handle_interrupt(i)
#define __handle_interrupt(i) \
	void isr_wrapper_##i(); \
	idt_set_descriptor(i, isr_wrapper_##i);

#define KEYBOARD_IV 9
#define PAGE_FAULT_IV 14
#define GPF_IV 13

#define LINE 28 /* line received interrupt */
#define SYSCALL 0x80 /* syscalls interrupt (for future use) */

struct idt_entry
{
	u16 addr_low pack;
	u16 code_segment pack;
	u8 reserved pack;
	u8 flags pack;
	u16 addr_high pack;
};

struct idt_ptr
{
	u16 limit pack;
	u32 addr pack;
};

// asmbits.s
void load_idt();

void idt_set_descriptor(int n, void *fptr);
void idt_install();
void emit_signal(int iv, void *data);
