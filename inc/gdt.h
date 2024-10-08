#pragma once

#include "lib.h"

// much information was taken from
// http://www.osdever.net/bkerndev/Docs/gdt.htm

struct gdt_entry
{
	u16 limit_low pack;
	u16 base_low pack;
	u8 base_middle pack;
	u8 access pack;
	u8 granularity pack;
	u8 base_high pack;
};

/* the actual structure we pass to lgdt
 * which describes all our GDT entries */
struct gdt_ptr
{
	u16 size pack;
	u32 addr pack;
};

void gdt_install();
void gdt_flush();

#define null 0,0,0,0
// see https://wiki.osdev.org/Global_Descriptor_Table
// for a description of each bit
#define kernel_code 0,0xFFFFFFFF,0b10011011,0b1100
#define kernel_data 0,0xFFFFFFFF,0b10010011,0b1100
#define user_code 0,0xFFFFFFFF,0b11111011,0b1100
#define user_data 0,0xFFFFFFFF,0b11110011,0b1100
