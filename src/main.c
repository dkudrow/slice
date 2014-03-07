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

char sa[] = "Greetings from global sa[]\n";
char *sp = "Greetings from global *sp\n";

/*
 * entry point to our operating system
 */
slice_main()
{
	int i, ret;

	gpio_function_select(16, 1);

	ret = fb_init();
	if (ret != 0) {
		error_solid();
	}

	timer_wait(7000000);


	console_init();

	printf("Slice OS is %u%% awesome%c\n", 101, '!');

	error_blink();
}
