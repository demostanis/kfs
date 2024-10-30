#pragma once

#include "video.h"

void putnbr_i32(int n);
void putnbr_i64(long long n);
void putnbrx_u32(unsigned int n, int size);
void putnbrx_u64(unsigned long long n, int size);

void putstr(char *s);
void putchar(char c);
void putcharpr(char c);
void putbytespr(unsigned char *s, int size);
void putbin(unsigned int n, int size);

void print_addr(unsigned char *addr, int lines);

void printk(const char *s, ...);
