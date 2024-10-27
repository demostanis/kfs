#include "kmalloc.h"

addr heap_top = KERNEL_HEAP_BEGIN;

void *heap_alloc_page()
{
	if (heap_top >= KERNEL_HEAP_END)
		return 0;

	void *page = pmem_alloc_page();
	if (page == 0)
		return 0;

	addr virt_addr = heap_top;
	if (map_phys_to_virt((addr)page, virt_addr) != 0)
		return 0;
	heap_top += PAGESIZE;

	return (void *)virt_addr;
}

void *heap_alloc_pages(int n)
{
	void *page = heap_alloc_page();
	if (page == 0)
		return 0;

	while (--n)
	{
		void *next = heap_alloc_page();
		if (next == 0)
			// TODO: free everything?
			return 0;
	}
	return page;
}

struct block *blocks;
struct block *first_free_block;
struct block *last_block;
usize n_blocks;

struct block *find_free_block(usize size)
{
	struct block *block = first_free_block;

	while (block)
	{
		if (block->is_free && block->size >= size)
			return block;
		block = block->next;
	}
	return 0;
}

void *kmalloc(usize size)
{
	assert(size != 0, "zero kmalloc size");

	int npages = 1 + size / PAGESIZE;
	struct block *block = find_free_block(size);
	if (block == 0)
		block = heap_alloc_pages(npages);
	warn(block == 0, "failed to allocate page in VM, OOM?");
	if (block == 0)
		return 0;

	block->magic = BLOCK_MAGIC;
	block->size = size;
	block->is_free = 0;
	block->next = 0;

	if (!blocks)
		blocks = block;
	if (last_block)
		last_block->next = block;
	last_block = block;
	if (first_free_block == block)
		first_free_block = block->next;

	n_blocks++;

	addr ptr = (addr)block + sizeof(struct block);
	if (ptr % ALIGNMENT != 0)
		ptr += ALIGNMENT - ptr % ALIGNMENT;

	usize next_page = BLOCK_PAGE_ADDR(block) + npages * PAGESIZE;
	usize needed_bytes = ALIGNMENT
		+ sizeof(struct block)
		+ 1; /* minimum kmalloc size */
	usize free_in_page = next_page - (ptr + size);
	if (needed_bytes < free_in_page)
	{
		// we can place another block in page
		struct block *next_block = (struct block *)(ptr + size);

		next_block->is_free = 1;
		next_block->size = free_in_page - needed_bytes;
		next_block->next = 0;

		block->next = next_block;

		if (!first_free_block)
			first_free_block = next_block;
	}
	else
		/* we'll have to allocate a new page on next call */
		;

	return (void *)ptr;
}

void kfree(void *ptr)
{
	struct block *block = 0;

	void *maybe_magic = ptr - sizeof(struct block);
	while (maybe_magic > ptr - sizeof(struct block) - ALIGNMENT)
	{
		if (*(int *)maybe_magic == BLOCK_MAGIC)
		{
			block = (struct block *)maybe_magic;
			break;
		}
		maybe_magic--;
	}

	assert(block != 0, "kfree: invalid pointer (corrupted block?)");

	block->is_free = 1;
	if (block < first_free_block)
		first_free_block = block;

	// TODO: defragment
}
