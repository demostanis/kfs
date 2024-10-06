#pragma once

#include "lib.h"

#define MULTIBOOT_BOOTLOADER_MAGIC 0x2BADB002

struct multiboot_info
{
	u32 flags pack;
	u32 mem_lower pack;
	u32 mem_upper pack;
	/* we don't care about other unused fields for now */
} pack;
