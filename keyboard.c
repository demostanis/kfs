#include "keyboard.h"

// http://www.osdever.net/bkerndev/Docs/keyboard.htm
char keymap[] = {
	0,  27, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=',
	'\b' /* Backspace */, 0,
	'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']',
	'\n' /* Enter key */, 0,
	'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';',
	'\'', '`', 0,
	'\\', 'z', 'x', 'c', 'v', 'b', 'n',
	'm', ',', '.', '/', 0, '*', 0, ' ',
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '-',
    0, 0, 0, '+', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

char *buf;
usize bufi;
int has_line;
int ignore_next;

void interrupt_handler_9()
{
	u8 scan_code = SCAN_CODE();

	if (ignore_next)
	{
		ignore_next = 0;
		goto end;
	}

	if (scan_code == 0xe0) // extended keys
		ignore_next = 1;

	if (RELEASED(scan_code))
	{
		char key = keymap[scan_code];
		if (key)
		{
			if (buf == 0)
			{
				buf = kmalloc(BUFFER_SIZE);
				if (buf == 0)
					MEMFAIL();
			}
			if (bufi >= BUFFER_SIZE)
			{
				buf = krealloc(buf, bufi + BUFFER_SIZE);
				if (buf == 0)
					MEMFAIL();
			}

			if (key == '\n')
			{
				buf[bufi] = 0;
				has_line = 1;
				putchar(key);
				goto end;
			}
			else if (key == '\b')
			{
				if (bufi == 0)
					bufi++;
				buf[bufi--] = 0;
				backspace();
				goto end;
			}

			putchar(key);

			buf[bufi++] = key;
		}
	}

end:
	ACK();
}

char *get_line()
{
	while (!has_line)
		__asm__ volatile("hlt");
	char *line = buf;
	buf = 0;
	bufi = 0;
	has_line = 0;
	return line;
}
