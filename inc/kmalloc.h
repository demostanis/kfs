#pragma once

#include "pmem.h"
#include "vmem.h"

#define KERNEL_HEAP_BEGIN 0xd0000000
#define KERNEL_HEAP_END 0xe0000000
#define ALIGNMENT 8

struct block
{
	struct block *next;
	usize size;
};

void *kmalloc(usize size);
