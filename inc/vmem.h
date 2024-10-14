#pragma once

#include "lib.h"
#include "errno.h"
#include "pmem.h"

// https://wiki.osdev.org/Paging for structure descriptions

#define NTABLES_PER_DIRECTORY 1024
struct table
{
	/* we only use bits addr, writable and present */
	/* (and probably user in the future) */
	u32 present : 1;
	u32 writable : 1;
	u32 user : 1;
	u32 _write_through : 1;
	u32 _cached : 1;
	u32 _accessed : 1;
	u32 _unused2 : 1;
	u32 _pagesize : 1;
	u32 _unused1 : 4;
	u32 addr : 20;
};

#define NPAGES_PER_TABLE 1024
struct page
{
	/* we only use bits addr, writable and present */
	/* (and probably user in the future) */
	u32 present : 1;
	u32 writable : 1;
	u32 user : 1;
	u32 _write_through : 1;
	u32 _cached : 1;
	u32 _accessed : 1;
	u32 _dirty : 1;
	u32 _pat : 1;
	u32 _global : 1;
	u32 _unused : 3;
	u32 addr : 20;
};

#define ADDR_TO_DIRECTORY_INDEX(addr) ((addr) >> 22 & 0x3ff)
#define ADDR_TO_TABLE_INDEX(addr) ((addr) >> 12 & 0x3ff)

// asmbits.s
void load_page_directory(volatile struct page_directory *);
void enable_paging();

void init_page_directory();

struct page_directory
{
	struct table tables[NTABLES_PER_DIRECTORY];
};

struct page_table
{
	struct page pages[NPAGES_PER_TABLE];
};
