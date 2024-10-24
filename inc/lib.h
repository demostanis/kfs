#pragma once

#include "types.h"
#include "printk.h"

char *strchr(const char *s, char c);
int strlen(const char *s);
void memset(void *buf, int c, usize n);
void bzero(void *buf, usize n);
void memmove(void *dst, void *src, usize n);

#if defined(DISABLE_ASSERTIONS) && !defined(RUNTESTS)
# define assert(cond, msg)
# define ensure(cond)
#else
# define assert(cond, msg)               \
{                                        \
	int ret = (cond);                    \
	if (!ret)                            \
		panic("assertion failed: " msg); \
}
# define ensure(cond) assert(cond, # cond)
# define warn(cond, msg)         \
{                                \
	int ret = (cond);            \
	if (ret)                     \
		printk("warning: " msg); \
}
#endif

__attribute__((noreturn)) void panic(char *msg);

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
