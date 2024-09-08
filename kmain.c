#define VIDEO_BADDR 0xB8000
#define VIDEO_WIDTH 80

#define outb(_port, _val) \
{ unsigned int port = _port; \
	unsigned short val = _val; \
	__asm__ volatile("outb %b0, %w1" \
			: : "a"(val), "Nd"(port) \
			: "memory"); }

void disable_cursor()
{
	outb(0x3D4, 0x0A);
	outb(0x3D5, 0x20);
}

void printk(const char *s)
{
	static volatile char *video = (volatile char *)VIDEO_BADDR;

	volatile char *line_begin = video;
	while (*s)
	{
		*video++ = *s++;
		*video++ = 0x1F;
	}
	video += (VIDEO_WIDTH*2)-(video-line_begin); // newline
}

void kmain()
{
	disable_cursor();

	printk("42");
}
