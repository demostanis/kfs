#pragma once

#include "video.h"

void putchar(char c);
void putnbr_i32(int n);
void putnbr_i64(long long n);
void putnbrx_u32(unsigned int n, int size);
void putnbrx_u64(unsigned long long n, int size);

void putcharpr(char c);
void putstrpr(char c);

void printk(const char *s, ...);
__attribute__((noreturn)) void panic(char *msg);
