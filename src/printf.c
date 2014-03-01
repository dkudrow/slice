/*
 * src/printf.c
 *
 * simple printf implementation
 */

#include <stdarg.h>

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
			putchar(48 + buf[i++]);

}

/*
 * print a signed decimal integer
 */
static void print_i(int n)
{
	int i;
	int buf[DEC_MAX];

	if (n < 0) {
		putchar('-');
		n *= -1;
	}
	for (i=1; i<=DEC_MAX; i++) {
		buf[DEC_MAX-i] = n % 10;
		n = (n) ? n/10 : 0;
	}
	for (i=0; buf[i]==0 && i<DEC_MAX-1; i++)
		;

	while (i < DEC_MAX)
		putchar(48 + buf[i++]);
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
			putchar(48 + buf[i++]);
		else
			putchar(55 + buf[i++]);
	}
}

/*
 * very basic formatted printing
 */
void printf(char *format_str, ...)
{
	char *s = format_str;
	va_list args;
	int arg_int;
	char *arg_s;

	/* get the format arguments and set the counter */
	va_start(args, format_str);

loop:
	switch (*s) {
		/* end of string */
		case '\0':
			break;

		/* found a format chacter */
		case '%':
			switch(*(++s)) {

				/* percent literal */
				case '%':
					putchar('%');
					break;

				/* unsigned decimal integer */
				case 'u':
					arg_int = va_arg(args, int);
					print_u(arg_int);
					break;
				/* decimal integer */
				case 'i':
				case 'd':
					arg_int = va_arg(args, int);
					print_i(arg_int);
					break;

				/* hex integer */
				case 'p':
				case 'x':
					arg_int = va_arg(args, int);
					print_x(arg_int);
					break;

				/* character */
				case 'c':
					arg_int = va_arg(args, int);
					putchar((char)arg_int);
					break;

				/* string */
				case 's':
					arg_s = va_arg(args, char *);
					while (*arg_s != '\0')
						putchar(*(arg_s++));
					break;
			}
			++s;
			goto loop;

		/* normal character */
		default:
			putchar(*s);
			++s;
			goto loop;
	}

}
