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

struct block *blocks;
struct block *last_block;
usize n_blocks;

void *kmalloc(usize size)
{
	struct block *block = heap_alloc_page();
	warn(block == 0, "failed to allocate page in VM, OOM?");
	if (block == 0)
		return 0;

	block->size = size;
	block->next = 0;

	if (!blocks)
		blocks = block;
	if (last_block)
		last_block->next = block;
	last_block = block;

	n_blocks++;

	u32 ptr = (u32)block + sizeof(struct block);
	if (ptr % ALIGNMENT != 0)
		ptr += ALIGNMENT - ptr % ALIGNMENT;

	heap_alloc_page(); // don't mind this little guy...
	return (void *)ptr;
}
