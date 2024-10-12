#pragma once

#include "types.h"
#include "printk.h"

char *strchr(const char *s, char c);
int strlen(const char *s);
void memset(void *buf, int c, usize n);
void bzero(void *buf, usize n);

__attribute__((noreturn)) void panic(char *msg);
void assert(int cond, char *msg);

void shutdown();

/* we pack each member to make sure tcc does
 * not fill our structure with more data for
 * optimizations. this however seems unneeded,
 * but it's left out as a safety measure in
 * case that's just specific to my machine
 * 😭 */
#define pack __attribute__((packed))

#define skip_bytes(st, bytes) \
	(((void *)st)+bytes)

#define offsetof(st, m) \
	((usize)&(((st *)0)->m))

#define len(t) \
	(sizeof t/sizeof(*t))
