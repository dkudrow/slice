/*
 * src/main.c
 *
 * entry point for OS
 */

#include "debug.h"
#include "gpio.h"
#include "timer.h"
#include "framebuffer.h"
#include "util.h"

char s[] = "Greetings from the console driver!";

/*
 * entry point to our operating system
 */
main()
{
	int i, ret;

	gpio_function_select(16, 1);

	ret = fb_init();
	if (ret != 0) {
		error_solid();
	}

	timer_wait(10000000);


	console_init();

	console_putc('S');
	console_putc('l');
	console_putc('i');
	console_putc('c');
	console_putc('e');

	console_putc('\n');

	console_putc('O');
	console_putc('S');

	error_blink();
}
