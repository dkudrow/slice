/*
 * src/console.c
 *
 * framebuffer console driver
 */

#include "console.h"
#include "framebuffer.h"
#include "timer.h"

/* TODO: hardcode the console for now */
struct console_t console = {
	.rows = 80,
	.cols = 24,
	.cur_row = 0,
	.cur_col = 0,
	.cur_pos = console.buf
};

/*
 * copy a string into the console buffer
 */
void console_write_str(char *str)
{
	int i = 0;

	while (str[i] != '\0') {
		*(console.cur_pos++) = str[i++];
	}
}

/*
 * write the console buffer to the screen
 */
void console_flush()
{
	char *p = console.buf;

	while (p < console.cur_pos) {
		//timer_wait(250000);
		fb_draw_char(console.cur_col++, console.cur_row, *(p++));
	}

	console.cur_pos = console.buf;
}
