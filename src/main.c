/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ *
 *
 * src/main.c
 *
 * Main entry point to Slice OS
 *
 * Author:	Daniel Kudrow (dkudrow@cs.ucsb.edu)
 * Date:	March 7 2014
 *
 * Copyright (c) 2014, Daniel Kudrow
 * All rights reserved, see LICENSE.txt for details.
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ *
 */

#include <debug.h>
#include <emmc.h>
#include <framebuffer.h>
#include <gpio.h>
#include <led.h>
#include <timer.h>
#include <util.h>

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
	kprintf("Console initialized, welcome to Slice.\n");

	emmc_init();

	kprintf("Done.\n");

	error_blink();
}
