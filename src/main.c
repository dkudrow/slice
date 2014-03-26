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

	/* timer_wait(7000000); */


	console_init();
	printf("Console initialized, welcome to Slice.\n");

	/*
	printf("Reading exception vector table...\n");
	for (i=0x0; i<0x20; i+=4)
		printf("0x%x: 0x%x\n", i, *(unsigned *)i);
	*/

	emmc_init();

	printf("Done.");

	error_blink();
}
