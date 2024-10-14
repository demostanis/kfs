#include "vmem.h"

#define PAGE_DIRECTORY 0xffc00000

volatile struct page_directory *virt_page_directory;

int map_phys_to_virt(u32 paddr, u32 vaddr)
{
	struct table *table = &virt_page_directory->tables[
		ADDR_TO_DIRECTORY_INDEX(vaddr)];
	if (!table->present)
	{
		pmem_info();
		void *new_table = pmem_alloc_page();
		if (new_table == 0)
			return -ENOMEM;

		table->present = 1;
		table->writable = 1;
		table->addr = (u32)new_table >> 12;
		bzero(PAGE_DIRECTORY + new_table, sizeof(table));
	}

	struct page_table *new_table =
		(struct page_table *)(PAGE_DIRECTORY + table->addr);
	struct page *new_page = &new_table->pages[ADDR_TO_TABLE_INDEX(vaddr)];

	new_page->present = 1;
	new_page->writable = 1;
	new_page->addr = paddr >> 12;

	return 0;
}

int map_many(u32 paddr, u32 vaddr, int size)
{
	int i = 0;
	while (i <= size)
	{
		int ret = map_phys_to_virt(paddr+i, vaddr+i);
		if (ret)
			return ret;
		i += PAGESIZE;
	}
	return 0;
}

int identity_map(u32 addr, int size)
{
	return map_many(addr, addr, size);
}

extern void *page_directory;

void init_page_directory()
{
	// last page table in the page directory (recursive paging)
	virt_page_directory = (volatile struct page_directory *)0xfffff000;

	// map kernel to higher half (which is already done
	// by the pre-kernel code in kernel.s)
	assert(map_many((u32)&kernel_begin, 0xc0000000, kernel_length) == 0,
			"failed to map kernel into VM");

	// TODO: maybe this should be done in kernel.s so panics
	// appear when this assertion fails?
	assert(map_many(0xb8000, 0xc00b8000, LINES*COLUMNS) == 0,
			"failed to map VGA into VM");

	// unmap the first 4MiB (unlike vcornill)
	*((u32 *)virt_page_directory) = 0;

	load_page_directory((struct page_directory *)&page_directory);
	enable_paging();
}
