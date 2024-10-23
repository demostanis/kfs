#include "pmem.h"

// Algorithm from http://brokenthorn.com/Resources/OSDev17.html

/* =~ 4GiB/PAGESIZE/sizeof(u32) */
u32 bitmap[32768] = { [ 0 ... 32767 ] = 0xffffffff };
int free_blocks = 0;

void bitmap_set_used(int bit)
{
	bitmap[bit / 32] |= (1 << bit % 32);
	free_blocks--;
}

void bitmap_set_unused(int bit)
{
	bitmap[bit / 32] &= ~(1 << bit % 32);
	free_blocks++;
}

int bitmap_is_used(int bit)
{
	return bitmap[bit / 32] & (1 << bit % 32);
}

int bitmap_first_free()
{
	u32 i = 0;

	while (i < len(bitmap))
	{
		u32 j = 0;

		if (bitmap[i] != 0xffffffff)
		{
			while (j < 32)
			{
				if ((bitmap[i] & 1 << j) == 0)
					return i * 32 + j;
				j++;
			}
		}
		i++;
	}
	return -1;
}

int bitmap_init_region(u32 addr, u32 length)
{
	u32 end = addr + length;

	while (addr + PAGESIZE <= end)
	{
		bitmap_set_unused(addr / PAGESIZE);
		addr += PAGESIZE;
	}

	bitmap_set_used(0); // NULL should not be allocated
}

int bitmap_deinit_region(u32 addr, u32 length)
{
	u32 end = addr + length;

	while (addr + PAGESIZE <= end)
	{
		bitmap_set_used(addr / PAGESIZE);
		addr += PAGESIZE;
	}
}

void *pmem_alloc_page()
{
	if (free_blocks <= 0)
		return 0;

	int free_page = bitmap_first_free();
	if (free_page == -1)
		return 0;

	bitmap_set_used(free_page);

	return (void *)(free_page * PAGESIZE);
}

void pmem_free_page(void *addr)
{
	u32 page = (u32)addr / PAGESIZE;
	bitmap_set_unused(page);
}

extern struct multiboot_info *mboot_info;

/* populated by the linker */
extern void *kernel_end;
extern void *kernel_begin;

void init_pmem_regions()
{
	usize i = 0;
	while (i < mboot_info->mmap_length)
	{
		struct mmap_entry *entry = (struct mmap_entry *)
			(mboot_info->mmap_addr + i);

		if (entry->type == 1 /* unused */)
			bitmap_init_region(entry->base_addr, entry->length);

		i += sizeof(struct mmap_entry);
	}

	bitmap_deinit_region(0, (u32)&kernel_end);
}

void pmem_info()
{
	printk("free blocks: %d", free_blocks);
	printk("free bytes: %d", free_blocks*PAGESIZE);
	printk("free memory: %dMiB", free_blocks*PAGESIZE/1024/1024);
}
