/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ *
 *
 * src/timer.c
 *
 * BCM2835 system timer
 *
 * Author:	Daniel Kudrow (dkudrow@cs.ucsb.edu)
 * Date:	March 7 2014
 *
 * Copyright (c) 2014, Daniel Kudrow
 * All rights reserved, see LICENSE.txt for details.
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ *
 *
 * The system timer is the BCM2835's global clock. It is laid out as
 * follows:
 *
 *	System timer base: 0x20003000
 *
 *	offset		function
 *	--------------------
 *	0x00		Control/status
 *	0x04		Counter (32 low bits)
 *	0x08		Counter (32 high bits
 *	0x0C		Compare 0
 *	0x10		Compare 1
 *	0x14		Compare 2
 *	0x18		Compare 3
 *
 * Note that this is NOT the timer used by the ARM Core! Actually it is,
 * but the ARM timer is accessed through the ARM processor at 0x2000B400
 * and is laid out a little differently. If we ever use it, I will add the
 * documentation for it here.
 * 
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ *
 */

#include <platform.h>
#include <util.h>

/*
 * get the current count
 */
unsigned timer_read()
{
	/* read the low 32 bits from the system clock register */
	return READ4(TIMER_CNTLO);
}

/*
 * busy wait
 */
void timer_wait(unsigned ticks)
{
	unsigned start, current;

	/* get the start count */
	start = READ4(TIMER_CNTLO);

	/* wait the specified number of ticks */
	do {
		current = READ4(TIMER_CNTLO);
	} while (current - start < ticks);
}
