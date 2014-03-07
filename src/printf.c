/*
 * src/printf.c
 *
 * allow the kernel to print directly to the console
 */

#include <stdarg.h>
#include "console.h"

#define DEC_MAX 10		/* max digitis in a decimal integer */
#define HEX_MAX 8		/* max digitis in a hex integer */

/*
 * print unsigned decimal integer
 */
static void print_u(unsigned n)
{
	int i;
	int buf[DEC_MAX];

	for (i=1; i<=DEC_MAX; i++) {
		buf[DEC_MAX-i] = n % 10;
		n = (n) ? n/10 : 0;
	}
	for (i=0; buf[i]==0 && i<DEC_MAX-1; i++)
		;

	while (i < DEC_MAX)
			console_putc(48 + buf[i++]);

}

/*
 * print a signed decimal integer
 */
static void print_i(int n)
{
	int i;
	int buf[DEC_MAX];

	if (n < 0) {
		console_putc('-');
		n *= -1;
	}
	for (i=1; i<=DEC_MAX; i++) {
		buf[DEC_MAX-i] = n % 10;
		n = (n) ? n/10 : 0;
	}
	for (i=0; buf[i]==0 && i<DEC_MAX-1; i++)
		;

	while (i < DEC_MAX)
		console_putc(48 + buf[i++]);
}

/*
 * print an unsigned hex integer
 */
static void print_x(unsigned n)
{
	int i;
	int buf[HEX_MAX];

	for (i=1; i<=HEX_MAX; i++) {
		buf[HEX_MAX-i] = n % 16;
		n = (n) ? n/16 : 0;
	}
	for (i=0; buf[i]==0 && i<HEX_MAX-1; i++)
		;

	while (i < HEX_MAX) {
		if (buf[i] < 10)
			console_putc(48 + buf[i++]);
		else
			console_putc(55 + buf[i++]);
	}
}

/*
 * simple formatted printing
 */
void printf(char *format_str, ...)
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

		++s;
		switch (*s) {
			case '%':
				console_putc('%');
				break;
			case 'c':
				arg_int = va_arg(arg_list, int);
				console_putc((char)arg_int);
				break;
			case 'u':
				arg_int = va_arg(arg_list, int);
				print_u(arg_int);
				break;
			case 'd':
			case 'i':
				arg_int = va_arg(arg_list, int);
				print_i(arg_int);
				break;
			case 'p':
			case 'x':
				arg_int = va_arg(arg_list, int);
				print_x(arg_int);
				break;
			case 's':
				arg_str = va_arg(arg_list, char *);
				while (*arg_str)
					console_putc(*arg_str++);
				break;
			case '\0':
				console_putc('?');
				return;
			default:
				console_putc('?');
		}
		++s;
	}
}
