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

char strcmp(const char *s1, const char *s2)
{
	while (*s1 && *s1 == *s2)
	{
		s1++;
		s2++;
	}
	return *s2 - *s1;
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

void memmove(void *dst, void *src, usize n)
{
	if (src < dst)
	{
		usize i = n;
		while (i-- > 0)
			((u8 *)dst)[i] = ((u8 *)src)[i];
		if (n)
			*dst = *src;
	}
	else
	{
		usize i = 0;
		while (i < n)
		{
			((u8 *)dst)[i] = ((u8 *)src)[i];
			i++;
		}
	}
}

#include "tests.h"

TESTS()
{
	ensure(strlen("hello") == 5);
	ensure(strlen("") == 0);

	ensure(strcmp("hello", "hello") == 0);
	ensure(strcmp("hello", "hellp") == 1);

	ensure(strcmp(strchr("hello world", 'w'), "world") == 0);

	{
		char dst[] = "hello brigitte";
		char src[] = "world";
		memmove(dst + 6, src, sizeof(src));
		ensure(strcmp(dst, "hello world") == 0);
	}

	{
		char dst[] = "world hello";
		char *src = dst + 6;
		memmove(dst, src, 5);
		ensure(strcmp(dst, "hello hello") == 0);
	}
}
