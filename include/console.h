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

#define CONS_BUF_SIZE 256
#define CONS_NUM_COLS 80
#define CONS_NUM_ROWS 40

/* Function prototypes */
void console_init();
void console_clear();
void console_putc(char c);

#endif /* CONSOLE_H */
