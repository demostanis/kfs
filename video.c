#include "video.h"

void clear_video()
{
	bzero((char *)VIDEO_BADDR, COLUMNS*LINES*2);
}

void disable_cursor()
{
	outb(0x3D4, 0x0A);
	outb(0x3D5, 0x20);
}

void __write(const char *s, int n)
{
	static volatile char *video = (volatile char *)VIDEO_BADDR;
	static volatile char *line_begin = (volatile char *)VIDEO_BADDR;

	int i = 0;
	while (*s && i < n)
	{
		write_serial(*s);
		if (*s == '\n')
		{
			video += (COLUMNS*2)-(video-line_begin);
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
