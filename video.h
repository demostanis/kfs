#pragma once

#include "io.h"
#include "stdarg.h"
#include "lib.h"

#define VIDEO_BADDR 0xB8000
#define VIDEO_WIDTH 80

void disable_cursor();
void printk(const char *s, ...);
