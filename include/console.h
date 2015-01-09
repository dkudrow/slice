/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ *
 *
 * include/console.h
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
 */

#ifndef CONSOLE_H
#define CONSOLE_H

#define CONSOLE_BUF_SIZE 256

/* console structure holds the current state of the screen */
struct console_t {
	unsigned rows;		/* number of rows */
	unsigned cols;		/* number of rows */
	unsigned cur_row;	/* current rows */
	unsigned cur_col;	/* current columns */
};

/* function prototypes */
void console_init();
void console_clear();
void console_putc(int c);

#endif /* CONSOLE_H */
