#pragma once

#include "types.h"

#define outb(_port, _val) \
{ u32 port = _port; \
	u16 val = _val; \
	__asm__ volatile("outb %b0, %w1" \
			: : "a"(val), "Nd"(port) \
			: "memory"); }
