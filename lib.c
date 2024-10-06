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
