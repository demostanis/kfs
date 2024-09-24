#include "video.h"

void disable_cursor()
{
	outb(0x3D4, 0x0A);
	outb(0x3D5, 0x20);
}

void __printkn(const char *s, int n)
{
	static volatile char *video = (volatile char *)VIDEO_BADDR;
	static volatile char *line_begin = (volatile char *)VIDEO_BADDR;

	int i = 0;
	while (*s && i < n)
	{
		write_serial(*s);
		if (*s == '\n')
		{
			video += (VIDEO_WIDTH*2)-(video-line_begin);
			line_begin = video;
			s++;
		}
		else
		{
			*video++ = *s++;
			*video++ = 0x1F;
		}
		i++;
	}
}

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

void putnbrx(int n)
{
	__printk("0x");
	__putnbrx_internal(n);
}

void __printfmt(char f, va_list *lst)
{
	switch (f)
	{
		case 'd':
		case 'i':
			putnbr(va_arg(*lst, int));
			break;
		case 'c':
			putchar(va_arg(*lst, char));
			break;
		case 'x':
			putnbrx(va_arg(*lst, int));
			break;
		case 's':
			__printk(va_arg(*lst, char *));
			break;
	}
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
			__printfmt(f[1], &lst);
			s = f+2;
		}
		else
			break;
	}
	va_end(lst);
	putchar('\n');
}
