#pragma once

#include "types.h"

#define SHT_SYMTAB 2

struct elf_shdr
{
	u32 name;
	u32 type;
	u32 flags;
	u32 addr;
	u32 offset;
	u32 size;
	u32 link;
	u32 info;
	u32 addralign;
	u32 entsize;
};

struct elf_sym
{
	u32 name;
	u32 value;
	u32 size;
	u8 info;
	u8 other;
	u16 shndx;
};

