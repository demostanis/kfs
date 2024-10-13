#pragma once

#include "lib.h"
#include "printk.h"
#include "multiboot.h"

#define PAGESIZE 4096

int bitmap_init_region(u32 addr, u32 length);
int bitmap_deinit_region(u32 addr, u32 length);

void *pmem_alloc_page();

void init_pmem_regions();

// debugging
void pmem_info();

/* populated by the linker */
extern void *kernel_end;
extern void *kernel_begin;
#define kernel_length ((u32)&kernel_end-(u32)&kernel_begin)
