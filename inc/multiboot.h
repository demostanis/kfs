#pragma once

#include "lib.h"

#define MULTIBOOT_BOOTLOADER_MAGIC 0x2BADB002
#define MULTIBOOT_INFO_MEM_MAP 0x00000040

/* offsetof(struct multiboot_info, mmap_length)-sizeof(multiboot_info->flags); */
#define UNUSED_FIELDS_PADDING 40 // we don't need them okay?

struct mmap_entry
{
	u32 size pack;
	u64 base_addr pack;
	u64 length pack;
	u32 type pack;
} pack;

struct multiboot_info
{
	u32 flags;
	/* 40 bytes skipped */
	u32 mmap_length;
	u32 mmap_addr;
} pack;

/* filled by kcommon */
extern struct multiboot_info *mboot_info;
