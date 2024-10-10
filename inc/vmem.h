#pragma once

#include "errno.h"
#include "pmem.h"

#define PD_NENTRIES 1024
#define PT_NENTRIES 1024

#define ENTRY_DIRECTORY_INDEX(entry) ((entry) >> 22 & 0x3ff)
#define ENTRY_TABLE_INDEX(entry) ((entry) >> 12 & 0x3ff)
#define ENTRY_TO_PHYS_ADDR(entry) ((entry) & 0xfffff000)
#define ENTRY_IS_PRESENT(entry) ((entry) >> 32 & 1)

#define ENTRY_SET_UNUSED(entry) ((entry) = 0b10)
#define ENTRY_SET_PRESENT(entry) ((entry) |= 0b11)
#define ENTRY_SET_PADDR(entry, paddr) ((entry) |= (paddr))

// asmbits.s
void load_page_directory(struct page_directory *);
void enable_paging();

void init_page_directory();

typedef u32 table;
typedef u32 page;

struct page_directory
{
	table tables[PD_NENTRIES];
};

struct page_table
{
	page pages[PT_NENTRIES];
};
