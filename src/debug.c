/*
 * src/debug.c
 *
 * some simple LED patters for debugging until the console works
 */

#include "gpio.h"
#include "timer.h"

/*
 * blink the LED forever
 */
error_blink()
{
	while (1) {
		gpio_set(16);
		timer_wait(250000);
		gpio_clear(16);
		timer_wait(250000);
	}
}

/*
 * turn on the LED forever
 */
error_solid()
{
	gpio_clear(16);
	while(1)
		;
}
