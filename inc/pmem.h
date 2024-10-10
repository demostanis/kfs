#pragma once

#include "lib.h"
#include "printk.h"

#define PAGESIZE 4096

int bitmap_init_region(u32 addr, u32 length);
int bitmap_deinit_region(u32 addr, u32 length);

void *pmem_alloc_page();

// debugging
void pmem_info();

/* populated by the linker */
extern void *kernel_end;
#define kernel_begin 0x100000
#define kernel_length &kernel_end-kernel_begin
