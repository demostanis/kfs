#include "printk.h"

void __printk(const char *s)
{
	__write(s, strlen(s));
}

void putchar(char c)
{
	__write(&c, 1);
}

void putcharpr(char c)
{
	if (c < '\x20' || c > '\x7f')
		c = '.';
	putchar(c);
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

void __putbin_internal(unsigned int n)
{
	if (n == 0 || n == 1)
		putchar(n % 2 + '0');
	else
	{
		__putbin_internal(n / 2);
		__putbin_internal(n % 2);
	}
}

void putbin(unsigned int n, int size)
{
	__printk("0b");

	int i = n, j = 0;
	while (i >>= 1)
		++j;
	int pad = size - j;
	while (pad-- > 1)
		__printk("0");

	__putbin_internal(n);
}

#define putnbr_for_type(t)     \
void putnbr_##t(t n)           \
{                              \
	if (n < 0)                 \
	{                          \
		putchar('-');          \
		n = -n;                \
	}                          \
	if (n >= 0 && n < 10)      \
		putchar(n % 10 + '0'); \
	else                       \
	{                          \
		putnbr_##t(n / 10);    \
		putnbr_##t(n % 10);    \
	}                          \
}
putnbr_for_type(i32)
putnbr_for_type(i64)
// TODO: unsigned?

#define putnbrx_for_t(t)                     \
void __putnbrx_internal_##t(t n)             \
{                                            \
	if (n >= 0 && n < 16)                    \
		putchar("0123456789abcdef"[n % 16]); \
	else                                     \
	{                                        \
		__putnbrx_internal_##t(n / 16);      \
		__putnbrx_internal_##t(n % 16);      \
	}                                        \
}                                            \
                                             \
void putnbrx_##t(t n, int size)              \
{                                            \
	__printk("0x");                          \
                                             \
	t i = n, j = 0;                          \
	while (i >>= 4)                          \
		++j;                                 \
	int pad = size - j;                      \
	while (pad-- > 1)                        \
		__printk("0");                       \
                                             \
	__putnbrx_internal_##t(n);               \
}
putnbrx_for_t(u32)
putnbrx_for_t(u64)
// TODO: signed?

int __printfmt(char *f, va_list *lst)
{
	static int size = 0;
	static int longlong = 0;

	int inc = 1;
	switch (*f)
	{
		case 'd':
		case 'i':
			if (longlong)
				putnbr_i64(va_arg(*lst, long long));
			else
				putnbr_i32(va_arg(*lst, int));
			break;
		case 'c':
			putchar(va_arg(*lst, char));
			break;
		case 'C':
			putcharpr(va_arg(*lst, char));
			break;
		case 'x':
			if (longlong)
				putnbrx_u64(va_arg(*lst, long long), size);
			else
				putnbrx_u32(va_arg(*lst, int), size);
			break;
		case 'p':
			putnbrx_u32(va_arg(*lst, int), 8);
			break;
		case 's':
			__printk(va_arg(*lst, char *));
			break;
		case 'b':
			putbin(va_arg(*lst, int), size);
			break;
		case 'B':
			putbytespr(va_arg(*lst, char *), size);
			break;
		case '%':
			putchar('%');
			inc = 1;
			break;
		case 'l':
			longlong = 1;
			inc += __printfmt(f+1, lst);
			longlong = 0;
			break;
		case '0' ... '9':
			size = size * 10 + *f - '0';
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
			__write(s, f-s);
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
	printk("Bytes: %4B %% %%%%", s);
	assert_serial("Bytes: ABC. % %%");

	printk("Hex: %x%x%x%x --", s[0], s[1], s[2], s[3]);
	assert_serial("Hex: 0x410x420x430x4 --");

	printk("BINARY: %32b %b", 0b01001100110011001010101010101001, 0b100000);
	assert_serial("BINARY: 0b01001100110011001010101010101001 0b100000");
}
