#pragma once

#include "lib.h"

#define IDT_NENTRIES 256

#define PIC1_DATA 0x21
#define PIC2_DATA 0xa1

// send EOI to PIC
#define ACK() outb(0x20, 0x20);

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
void isr_wrapper();

void idt_install();
