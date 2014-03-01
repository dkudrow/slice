/*
 * test/framebuffer.c
 *
 * dummy framebuffer to test out the console/tty drivers
 */

#include <stdio.h>

fb_init()
{
}

fb_test()
{
}

fb_border()
{
}

fb_draw_char(unsigned x, unsigned y, unsigned c)
{
	if (x == 0)
		putchar('\n');
	if (c < 32)
		c = ' ';
	putchar(c);
}
