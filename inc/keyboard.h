#pragma once

#include "lib.h"
#include "kmalloc.h"
#include "idt.h"

#define SCAN_CODE() inb(0x60);
#define RELEASED(code) (!(code & 0x80))
#define MEMFAIL() panic("no more memory to store keypresses, please downnload more RAM");

#define BUFFER_SIZE 1024

char *get_line();
