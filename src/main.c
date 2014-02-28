/*
 * src/main.c
 *
 * entry point for OS
 */

#include "debug.h"
#include "gpio.h"
#include "timer.h"
#include "framebuffer.h"

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

	console_write_str(s);
	console_flush();

	error_blink();
}
