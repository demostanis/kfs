#include "serial.h"

#define PORT 0x3F8 // COM1

void write_serial(char c)
{
	outb(PORT, c);
}
