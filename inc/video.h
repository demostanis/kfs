#pragma once

#include "io.h"
#include "stdarg.h"
#include "lib.h"
#include "serial.h"

#define VIDEO_BADDR 0xB8000
#define VIDEO_WIDTH 80

#define COLUMNS 80
#define LINES 25

void disable_cursor();
void clear_video();
void __write(const char *s, int n);