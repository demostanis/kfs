#include "vmem.h"

struct page_directory *page_directory;

int map_phys_to_virt(u32 paddr, u32 vaddr)
{
	struct table *table = &page_directory->tables[
		ADDR_TO_DIRECTORY_INDEX(vaddr)];
	if (!table->present)
	{
		void *new_table = pmem_alloc_page();
		if (new_table == 0)
			return -ENOMEM;
		bzero(new_table, sizeof(table));

		table->present = 1;
		table->writable = 1;
		table->addr = (u32)new_table >> 12;
	}

	struct page_table *new_table =
		(struct page_table *)(table->addr << 12);
	struct page *new_page = &new_table->pages[ADDR_TO_TABLE_INDEX(vaddr)];

	new_page->present = 1;
	new_page->writable = 1;
	new_page->addr = paddr >> 12;

	return 0;
}

void map_many(u32 paddr, u32 vaddr, int size)
{
	int i = 0;
	while (i <= size)
	{
		map_phys_to_virt(paddr+i, vaddr+i);
		i += PAGESIZE;
	}
}

void identity_map(u32 addr, int size)
{
	map_many(addr, addr, size);
}

void init_page_directory()
{
	page_directory = pmem_alloc_page();
	assert(page_directory != 0,
			"failed to allocate memory for page directory");

	// map kernel to higher half (which is already done
	// by the pre-kernel code in kernel.s)
	assert(map_many((u32)&kernel_begin, 0xc0000000, kernel_length) == 0,
			"failed to map kernel into VM");

	assert(map_many(0xb8000, 0xc00b8000, LINES*COLUMNS) == 0,
			"failed to VGA into VM");

	load_page_directory(page_directory);
	enable_paging();
}
