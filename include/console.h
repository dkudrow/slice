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
	unsigned char *cur_pos;	/* pointer to last buffered character */
	unsigned char buf[CONSOLE_BUF_SIZE];	/* buffer input */
};

/* function prototypes */
void console_write_str(char *str);
void console_flush();

#endif /* CONSOLE_H */
