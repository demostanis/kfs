#include "video.h"

volatile char *video = VIDEO_BADDR;
volatile char *line_begin = VIDEO_BADDR;

void clear_video()
{
	video = VIDEO_BADDR;
	line_begin = VIDEO_BADDR;
	bzero((char *)VIDEO_BADDR, VIDEO_LENGTH);
}

void disable_cursor()
{
	outb(0x3D4, 0x0A);
	outb(0x3D5, 0x20);
}

extern int user_input_start;

void backspace()
{
	if (video > line_begin + user_input_start * 2)
	{
		write_serial_str("\b \b");
		*video-- = 0;
		*video-- = 0;
	}
}

void __write(const char *s, int n)
{
	int i = 0;
	while (*s && i < n)
	{
		if (*s == '\n')
		{
			write_serial(*s);
			video += (LINE_LENGTH)-(video-line_begin);
			line_begin = video;
		}
		else
		{
			write_serial(*s);
			*video++ = *s;
			*video++ = 0x1F;
		}
		s++;

		if (video == VIDEO_EADDR)
		{
			memmove((void *)VIDEO_BADDR,
					(void *)VIDEO_BADDR + LINE_LENGTH,
					VIDEO_LENGTH - LINE_LENGTH);
			video -= LINE_LENGTH;
			line_begin = video;
			bzero((void *)video, LINE_LENGTH);
		}
		i++;
	}
}
