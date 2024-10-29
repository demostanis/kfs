#pragma once

#include "lib.h"
#include "elf.h"

#define MULTIBOOT_BOOTLOADER_MAGIC 0x2BADB002
#define MULTIBOOT_INFO_MEM_MAP 0x00000040
#define MULTIBOOT_INFO_ELF_SHDR 0x00000020

/* offsetof(struct multiboot_info, mmap_length)-sizeof(multiboot_info->flags); */
#define UNUSED_FIELDS_PADDING 24 // we don't need them okay?

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
	/* 28 bytes skipped */
	struct {
		u32 num;
		u32 size;
		u32 addr;
		u32 shndx;
	} elf_sections;
	u32 mmap_length;
	u32 mmap_addr;
} pack;

/* filled by kcommon */
extern struct multiboot_info *mboot_info;
