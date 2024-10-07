#pragma once

#include "types.h"

#define outb(_port, _val) \
{ u32 port = _port; \
	u16 val = _val; \
	__asm__ volatile("outb %b0, %w1" \
			: : "a"(val), "Nd"(port) \
			: "memory"); }

#define inb(_port) \
({ u8 ret; \
	u32 port = _port; \
	__asm__ volatile("inb %w1, %b0" \
			: "=a"(ret) \
			: "Nd"(port) \
			: "memory"); \
		return ret; })
