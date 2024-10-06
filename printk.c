#include "printk.h"

void __printk(const char *s)
{
	__printkn(s, strlen(s));
}

void putchar(char c)
{
	char buf[2];

	buf[0] = c;
	buf[1] = 0;
	__printk(buf);
}

void putcharpr(char c)
{
	if (c < '\x20' || c > '\x7f')
		c = '.';
	char buf[2];

	buf[0] = c;
	buf[1] = 0;
	__printk(buf);
}

void putbytespr(unsigned char *s, int size)
{
	int i = 0;

	while (i < size)
	{
		putcharpr(s[i]);
		i++;
	}
}

void putnbr(int n)
{
	if (n >= 0 && n < 10)
		putchar(n % 10 + '0');
	else
	{
		putnbr(n / 10);
		putnbr(n % 10);
	}
}

void __putnbrx_internal(int n)
{
	if (n >= 0 && n < 16)
		putchar("0123456789abcdef"[n % 16]);
	else
	{
		__putnbrx_internal(n / 16);
		__putnbrx_internal(n % 16);
	}
}

void putnbrx(int n, int size)
{
	__printk("0x");

	int i = n, j = 0;
	while (i /= 16)
		++j;
	int pad = size - j;
	while (pad-- > 1)
		__printk("0");

	__putnbrx_internal(n);
}

int __printfmt(char *f, va_list *lst)
{
	static int size = 0;

	int inc = 1;
	switch (*f)
	{
		case 'd':
		case 'i':
			putnbr(va_arg(*lst, int));
			break;
		case 'c':
			putchar(va_arg(*lst, char));
			break;
		case 'C':
			putcharpr(va_arg(*lst, char));
			break;
		case 'x':
			putnbrx(va_arg(*lst, int), size);
			break;
		case 's':
			__printk(va_arg(*lst, char *));
			break;
		case 'B':
			putbytespr(va_arg(*lst, char *), size);
			break;
		case '0' ... '9':
			size = *f - '0';
			inc += __printfmt(f+1, lst);
			size = 0;
			break;
	}
	return inc;
}

void printk(const char *s, ...)
{
	va_list lst;

	va_start(lst, s);
	while (*s)
	{
		char *f = strchr(s, '%');
		if (f)
			__printkn(s, f-s);
		else
			__printk(s);
		if (f)
		{
			int inc = __printfmt(f+1, &lst);
			s = f+inc+1;
		}
		else
			break;
	}
	va_end(lst);
	putchar('\n');
}

#include "tests.h"

TESTS()
{
	printk("Hello, world!");
	assert_serial("Hello, world!");

	printk("Here's a randomly chosen number: %d", 4);
	assert_serial("Here's a randomly chosen number: 4");

	printk("Here are many numbers: %d %d %d %d", 12, 42, 71897, 0);
	assert_serial("Here are many numbers: 12 42 71897 0");

	char *s = "ABC\x4";
	printk("Bytes: %4B", s);
	assert_serial("Bytes: ABC.");

	printk("Hex: %x%x%x%x", s[0], s[1], s[2], s[3]);
	assert_serial("Hex: 0x410x420x430x4");
}
