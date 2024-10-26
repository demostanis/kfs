#include "vmem.h"

volatile struct page_directory *virt_page_directory;

int map_phys_to_virt(addr paddr, addr vaddr)
{
	struct table *table = &virt_page_directory->tables[
		ADDR_TO_DIRECTORY_INDEX(vaddr)];
	if (!table->present)
	{
		void *new_table = pmem_alloc_page();
		if (new_table == 0)
			return -ENOMEM;

		table->present = 1;
		table->writable = 1;
		table->addr = (u32)new_table >> 12;

		bzero((void *)ADDR_TO_VIRT_PAGE_TABLE(vaddr), sizeof(struct page_table));
	}

	struct page *new_page =
		(struct page *)ADDR_TO_VIRT_PAGE(vaddr);
	warn(new_page->present, "page already mapped");

	new_page->present = 1;
	new_page->writable = 1;
	new_page->addr = paddr >> 12;

	return 0;
}

void unmap_page(u32 vaddr)
{
	struct page_table *table =
		(struct page_table *)ADDR_TO_VIRT_PAGE_TABLE(vaddr);
	struct page *page = &table->pages[ADDR_TO_TABLE_INDEX(vaddr)];
	warn(!page->present, "page not mapped");

	page->present = 0;
	page->addr = 0;

	int i = 0;
	while (i < NPAGES_PER_TABLE)
	{
		if (table->pages[i].present)
			break;
		i++;
	}
	if (i == NPAGES_PER_TABLE)
	{
		struct table *empty_table = &virt_page_directory->tables[
			ADDR_TO_DIRECTORY_INDEX(vaddr)];
		pmem_free_page((void *)empty_table->addr);

		empty_table->present = 0;
		empty_table->addr = 0;
	}
}

int map_many(u32 paddr, u32 vaddr, usize size)
{
	usize i = 0;
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

void unmap_until(u32 vaddr, usize size)
{
	usize i = 0;
	while (i < size)
	{
		unmap_page(vaddr+i);
		i += PAGESIZE;
	}
}

extern void *page_directory;

void init_page_directory()
{
	// last page table in the page directory (recursive paging)
	virt_page_directory = (volatile struct page_directory *)0xfffff000;

	// unmap the first 4MiB (unlike vcornill)
	unmap_until(0, 0x100000);

	// map kernel to higher half (which is already done
	// by the pre-kernel code in kernel.s)
	assert(map_many((u32)&kernel_begin, 0xc0000000, kernel_length) == 0,
			"failed to map kernel into VM");

	// TODO: maybe this should be done in kernel.s so panics
	// appear when this assertion fails?
	assert(map_many(0xb8000, (addr)VIDEO_BADDR, LINES*COLUMNS) == 0,
			"failed to map VGA into VM");

	load_page_directory((struct page_directory *)&page_directory);
	enable_paging();
}
