/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ *
 *
 * src/kprintf.c
 *
 * Print kernel messages
 *
 * Author:	Daniel Kudrow (dkudrow@cs.ucsb.edu)
 * Date:	March 7 2014
 *
 * Copyright (c) 2014, Daniel Kudrow
 * All rights reserved, see LICENSE.txt for details.
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ *
 */

#include <stdarg.h>
#include <console.h>

#define MAX_DIGITS 11	/* max octal digits in an integer */

static const char digits[] = { '0', '1', '2', '3', '4', '5', '6', '7', '8',
	'9', 'A', 'B', 'C', 'D', 'E', 'F' };

/*
 * Format an integer to hexadecimal, signed decimal or unsigned decimal
 */
static void format_int(int base, unsigned uval)
{
	int i=0;
	int buf[MAX_DIGITS];

	if (base < 0 && (int)uval < 0) {
		console_putc('-');
		uval = -1*(int)uval;
		base *= -1;
	}

	do {
		buf[i++] = uval % base;
		uval /= base;
	} while (uval != 0);

	do {
		console_putc(digits[buf[--i]]);
	} while (i);
}

/*
 * Formatted printing for the kernel
 */
void kprintf(char *format_str, ...)
{
	char *s = format_str;
	va_list arg_list;
	int arg_int;
	char *arg_str;

	/* get the format string arguments */
	va_start(arg_list, format_str);

	while (*s) {
		/* print a normal character */
		if (*s != '%') {
			console_putc(*s++);
			continue;
		}

		/* print a format string */
		++s;
		switch (*s) {
			/* percent literal */
			case '%':
				console_putc('%');
				break;
			/* character */
			case 'c':
				arg_int = va_arg(arg_list, int);
				console_putc(arg_int);
				break;
			/* unsigned decimal integer */
			case 'u':
				arg_int = va_arg(arg_list, int);
				/*format_int(arg_int);*/
				format_int(10, arg_int);
				break;
			/* signed decimal integer */
			case 'd':
			case 'i':
				arg_int = va_arg(arg_list, int);
				/*print_i(arg_int);*/
				format_int(-10, arg_int);
				break;
			/* unsigned hexadecimal integer */
			case 'p':
			case 'x':
				arg_int = va_arg(arg_list, int);
				/*print_x(arg_int);*/
				format_int(16, arg_int);
				break;
			/* string */
			case 's':
				arg_str = va_arg(arg_list, char *);
				while (*arg_str)
					console_putc(*arg_str++);
				break;
			/* invalid format string */
			case '\0':
				console_putc('?');
				return;
			default:
				console_putc('?');
		}
		++s;
	}
}
