#include "vmem.h"

struct page_directory page_directory;

int map_phys_to_virt(u32 paddr, u32 vaddr)
{
	table *entry = &page_directory.tables[
		ENTRY_DIRECTORY_INDEX(vaddr)];
	if (!ENTRY_IS_PRESENT(*entry))
	{
		void *new_table = pmem_alloc_page();
		if (new_table == 0)
			return -ENOMEM;
		bzero(new_table, sizeof(table));

		ENTRY_SET_PRESENT(*entry);
		ENTRY_SET_PADDR(*entry, (u32)new_table);
	}

	struct page_table *new_table =
		(struct page_table *)ENTRY_TO_PHYS_ADDR(*entry);
	page *new_page = &new_table->pages[ENTRY_TABLE_INDEX(vaddr)];

	ENTRY_SET_PRESENT(*new_page);
	ENTRY_SET_PADDR(*new_page, paddr);

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
	int i = 0;
	while (i < PD_NENTRIES)
	{
		ENTRY_SET_UNUSED(page_directory.tables[i]);
		i++;
	}

	// map important memory regions like VGA:
	// https://i.sstatic.net/A8gMs.png
	identity_map(0, 0x1000000);

	// map kernel to 0xc0000000 (higher half)
	map_many(kernel_begin, 0xc0000000, (int)kernel_length);

	load_page_directory((struct page_directory *)page_directory);
	enable_paging();
}
