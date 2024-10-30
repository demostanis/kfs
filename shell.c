#include "shell.h"

char prompt[] = "kfs # ";
int user_input_start = sizeof(prompt)-1; // used by keyboard.c

void interrupt_handler_28(char *line) // line received handler
{
	putstr(line);
	if (*line)
		putchar('\n');

	if (strcmp(line, "panic") == 0)
		panic("triggered panic");
	else if (strcmp(line, "fault") == 0)
		*((volatile char *)0x0) = 0;

	kfree(line);
	putstr(prompt);
}

void init_shell()
{
	putstr(prompt);
}
