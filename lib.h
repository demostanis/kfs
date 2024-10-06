#pragma once

#include "types.h"

char *strchr(const char *s, char c);
int strlen(const char *s);
void memset(void *buf, int c, usize n);
void bzero(void *buf, usize n);

void shutdown();

/* filled by kcommon */
extern struct multiboot_info *multiboot_info;

/* we pack each member to make sure tcc does
 * not fill our structure with more data for
 * optimizations. this however seems unneeded,
 * but it's left out as a safety measure in
 * case that's just specific to my machine
 * 😭 */
#define pack __attribute__((packed))
