#pragma once

#include "pmem.h"
#include "vmem.h"

#define KERNEL_HEAP_BEGIN 0xd0000000
#define KERNEL_HEAP_END 0xe0000000
#define ALIGNMENT 8

#define BLOCK_MAGIC 0xeeb9d4c1

#define BLOCK_PAGE_ADDR(block) (((addr)block) & 0xfffff000)

struct block
{
	int magic;
	struct block *next;
	int is_free;
	usize size;
};

void *kmalloc(usize size);
void kfree(void *ptr);
