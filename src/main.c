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

#define MODULE MAIN

#include <emmc.h>
#include <framebuffer.h>
#include <gpio.h>
#include <led.h>
#include <log.h>
#include <timer.h>
#include <util.h>

/*
 * entry point to our operating system
 */
slice_main()
{
	int i, ret;

	/* prepare LED pin */
	gpio_function_select(16, 1);

	/* initialize framebuffer */
	ret = fb_init();
	if (ret != 0) {
		error_blink();
	}

	/* initialize console */
	console_init();
	kprintf("Console initialized, welcome to Slice.\n");

	/* initialize SD card */
	emmc_init();

	kprintf("Done.\n");

	error_solid();
}
