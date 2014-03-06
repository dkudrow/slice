/*
 * src/console.c
 *
 * framebuffer console driver
 */

#include "console.h"
#include "framebuffer.h"
#include "timer.h"

struct console_t console;

/*
 * initialize a console
 */
void console_init()
{
	console.rows = 80;
	console.cols = 24;
	console.cur_row = 0;
	console.cur_col = 0;
}

/*
 * write one character onto the console
 */
void console_putc(int c)
{
	switch (c) {
		case '\n':
			++console.cur_row;
			console.cur_col = 0;
			break;
		default:
			fb_draw_char(console.cur_col, console.cur_row, (char)c);
			if (console.cur_col == console.cols-1) {
				++console.cur_row;
				console.cur_col = 0;
			} else {
				++console.cur_col;
			}
	}
}
