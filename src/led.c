/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ *
 *
 * src/led.c
 *
 * some simple LED patterns for debugging until the console works
 *
 * Author:	Daniel Kudrow (dkudrow@cs.ucsb.edu)
 * Date:	March 7 2014
 *
 * Copyright (c) 2014, Daniel Kudrow
 * All rights reserved, see LICENSE.txt for details.
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ *
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
