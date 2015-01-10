#include <stdio.h>

#define CONS_SIZE 1024

int pos;
char buf[CONS_SIZE+1];

char *dummy_console_reset()
{
	pos = 0;
	buf[0] = '\0';
	buf[CONS_SIZE] = '\0';
	return buf;
}

void console_putc(char c)
{
	buf[pos] = c;
	pos = (pos + 1) % CONS_SIZE;
	buf[pos] = '\0';
}
