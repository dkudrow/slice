/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ *
 *
 * src/console.c
 *
 * Console driver
 *
 * Author:	Daniel Kudrow (dkudrow@cs.ucsb.edu)
 * Date:	March 7 2014
 *
 * Copyright (c) 2014, Daniel Kudrow
 * All rights reserved, see LICENSE.txt for details.
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

#include <console.h>
#include <framebuffer.h>

/*
 * one conosle to rule them all
 */
struct console_t {
	unsigned rows;		/* number of rows */
	unsigned cols;		/* number of rows */
	unsigned cur_row;	/* current rows */
	unsigned cur_col;	/* current columns */
} console;


/*
 * Initialize a console
 */
void console_init()
{
	/* TODO fit console to framebuffer */
	console.cols = CONS_NUM_COLS;
	console.rows = CONS_NUM_ROWS;
	console.cur_row = 0;
	console.cur_col = 0;
}

/*
 * Clear the screen
 */
void console_clear()
{
	fb_scroll(console.rows, console.rows);
}

/*
 * Write one character onto the console
 */
void console_putc(char c)
{
	if (console.cur_row == console.rows) {
		fb_scroll(console.rows, 1);
		--console.cur_row;
	}

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
