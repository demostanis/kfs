#pragma once

#include "io.h"
#include "stdarg.h"
#include "lib.h"
#include "serial.h"

#define COLUMNS 80
#define LINES 25

#define VIDEO_BADDR ((volatile char *)0xc00b8000)
#define VIDEO_LENGTH (COLUMNS*LINES*2)
#define VIDEO_EADDR ((volatile char *)VIDEO_BADDR+VIDEO_LENGTH)

void disable_cursor();
void clear_video();
void __write(const char *s, int n);
