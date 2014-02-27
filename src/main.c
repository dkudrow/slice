/*
 * src/main.c
 *
 * entry point for OS
 */

#include "debug.h"
#include "gpio.h"
#include "timer.h"
#include "framebuffer.h"

/*
 * entry point to our operating system
 */
main()
{
	int i, ret;
	unsigned test;

	gpio_function_select(16, 1);

	ret = fb_init();
	if (ret != 0) {
		error_blink();
	}

	console_draw_char(0, 10, 65);
	console_draw_char(1, 10, 66);
	console_draw_char(2, 10, 67);
	console_draw_char(3, 10, 68);

	error_solid();
}
