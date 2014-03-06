/*
 * include/console.h
 *
 * console driver
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
void console_putc(int c);

#endif /* CONSOLE_H */
