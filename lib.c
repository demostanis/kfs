#include "lib.h"

char *strchr(const char *s, char c)
{
	while (*s)
	{
		if (*s == c)
			return (char *)s;
		s++;
	}
	if (c == 0)
		return (char *)s;
	return (char *)0;
}

int strlen(const char *s)
{
	int i = 0;
	while (s[i])
		i++;
	return i;
}

void memset(void *buf, int c, usize n)
{
	usize i = 0;

	while (i < n)
		((char *)buf)[i++] = c;
}

void bzero(void *buf, usize n)
{
	memset(buf, 0, n);
}

__attribute__((noreturn)) void panic(char *msg)
{
	printk("panic!");
	printk(msg);
	shutdown();
}

void assert(int cond, char *msg)
{
#ifdef ASSERTIONS
	if (!cond)
		panic(msg);
#endif
}
