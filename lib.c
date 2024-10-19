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

__attribute__((noreturn)) void panic(char *msg)
{
	printk("panic!");
	printk(msg);
	shutdown();
	// TODO: stack trace
	// TODO: maybe this should loop instead of shutting down?
	// TODO: this should write to 0xb8000 if paging isn't enabled,
	// maybe do a strcpy since we don't use formats anyway?
}

#include "tests.h"

TESTS()
{
	ensure(strlen("hello") == 5);
	ensure(strlen("") == 0);

	ensure(strcmp("hello", "hello") == 0);
	ensure(strcmp("hello", "hellp") == 1);

	ensure(strcmp(strchr("hello world", 'w'), "world") == 0);
}
