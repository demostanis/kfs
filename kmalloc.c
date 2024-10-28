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

struct block *find_block(void *ptr)
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

	return block;
}

// get the user usable address after a block
addr block_ptr(struct block *block)
{
	addr ptr = (addr)block + sizeof(struct block);
	if (ptr % ALIGNMENT != 0)
		ptr += ALIGNMENT - ptr % ALIGNMENT;
	return ptr;
}

void *kmalloc(usize size)
{
	assert(size != 0, "zero kmalloc size");

	int npages = 1 + size / PAGESIZE;
	struct block *block = find_free_block(size);
	if (block == 0)
	{
		block = heap_alloc_pages(npages);
		if (block)
			block->prev = 0;
	}
	warn(block == 0, "failed to allocate page in VM, OOM?");
	if (block == 0)
		return 0;

	block->magic = BLOCK_MAGIC;
	block->size = size;
	block->is_free = 0;
	block->next = 0;

	if (!blocks)
		blocks = block;
	if (first_free_block == block)
		first_free_block = block->next;

	addr ptr = block_ptr(block);

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
		next_block->prev = block;

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
	if (ptr == 0)
		return;

	struct block *block = find_block(ptr);
	assert(block != 0, "kfree: invalid pointer (corrupted block?)");

	block->is_free = 1;
	if (block < first_free_block)
		first_free_block = block;

	// defragment
	struct block *next_block = block->next;
	if (next_block && next_block->is_free)
	{
		block->size += next_block->size + sizeof(struct block);
		block->next = next_block->next;
	}
	struct block *prev_block = block->prev;
	if (prev_block && prev_block->is_free)
	{
		prev_block->size += block->size + sizeof(struct block);
		prev_block->next = block->next;
	}
}

void *krealloc(void *ptr, usize new_size)
{
	struct block *block = find_block(ptr);
	assert(block != 0, "krealloc: invalid pointer");

	void *new_ptr = kmalloc(new_size);
	if (new_ptr == 0)
		return 0;
	memmove(new_ptr, (void *)block_ptr(block), block->size);
	kfree(ptr);
	return new_ptr;
}

#include "tests.h"

/* tests utils */
usize n_free_blocks()
{
	struct block *block = blocks;
	usize n = 0;

	while (block)
	{
		if (block->is_free)
			n++;
		block = block->next;
	}
	return n;
}

usize n_used_blocks()
{
	struct block *block = blocks;
	usize n = 0;

	while (block)
	{
		if (!block->is_free)
			n++;
		block = block->next;
	}
	return n;
}

usize ksize(void *ptr)
{
	struct block *block = find_block(ptr);
	ensure(block != 0);
	return block->size;
}

#define abs(n) ((n) < 0 ? -(n) : (n))

TESTS()
{
	void *ptr = kmalloc(16);
	ensure(n_used_blocks() == 1);
	kfree(ptr);
	ensure(n_free_blocks() == 1);

	kfree(kmalloc(PAGESIZE));

	int i = 1;
	while (i < 100)
		kfree(kmalloc(i++));
	ensure(n_used_blocks() == 0);

	void *ptr1 = kmalloc(16);
	void *ptr2 = kmalloc(16);
	ensure(abs(ptr2 - ptr1) < PAGESIZE);
	kfree(ptr1);
	kfree(ptr2);

	void *ptr3 = kmalloc(32);
	ensure(ptr1 == ptr3);
	kfree(ptr3);

	void *ptr4 = kmalloc(10000);
	ensure(ksize(ptr4) == 10000);
	memset(ptr4, 'A', 10000);
	kfree(ptr4);

	void *ptr5 = kmalloc(2);
	ptr5 = krealloc(ptr5, 60);
	ensure(ksize(ptr5) == 60);
	kfree(ptr5);

	kfree(0);

	// valgrind!!!!!!!!!!
	ensure(n_used_blocks() == 0);
}
