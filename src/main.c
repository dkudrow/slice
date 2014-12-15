/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ *
 *
 * src/main.c
 *
 * main entry point to Slice OS
 *
 * Author:	Daniel Kudrow (dkudrow@cs.ucsb.edu)
 * Date:	March 7 2014
 *
 * Copyright (c) 2014, Daniel Kudrow
 * All rights reserved, see LICENSE.txt for details.
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ *
 */

#include "debug.h"
#include "emmc.h"
#include "framebuffer.h"
#include "gpio.h"
#include "led.h"
#include "timer.h"
#include "util.h"
#include "filesystem.h"

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

	printf("Done.\n");

        fat32_init();
        fat32_dump_part_table();

	error_blink();
}
