/*
 * src/main.c
 *
 * entry point for OS
 */

#include "debug.h"
#include "emmc.h"
#include "framebuffer.h"
#include "gpio.h"
#include "led.h"
#include "timer.h"
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

	console_init();
	printf("Console initialized, welcome to Slice.\n");

	emmc_init();

	printf("Done.");

	error_blink();
}
