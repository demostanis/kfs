#pragma once

#include "io.h"
#include "stdarg.h"
#include "lib.h"
#include "serial.h"

#define VIDEO_BADDR 0xB8000
#define VIDEO_WIDTH 80

void disable_cursor();
void __printkn(const char *s, int n);
