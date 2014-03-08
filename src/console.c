/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ *
 *
 * src/console.c
 *
 * console driver
 *
 * Author:	Daniel Kudrow (dkudrow@cs.ucsb.edu)
 * Date:	March 7 2014
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ *
 *
 * The console driver interacts directly with the framebuffer to draw text
 * to the screen in an organized manner. It handles text wrapping, the
 * position of the cursor, scrolling and similar niceties.
 *
 * For now there is one console however there is no reason that we can add
 * multiple virtual consoles down the line.
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ *
 */

#include "console.h"
#include "framebuffer.h"
#include "timer.h"

/*
 * one conosle to rule them all
 */
struct console_t console;

/*
 * initialize a console
 */
void console_init()
{
	console.cols = 80;
	console.rows = 24;
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
