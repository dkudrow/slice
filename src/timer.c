/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ *
 *
 * src/timer.c
 *
 * BCM2835 system timer
 *
 * Author:	Daniel Kudrow (dkudrow@cs.ucsb.edu)
 * Date:	March 7 2014
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ *
 *
 * The system timer is the BCM2835's global clock. It is laid out as
 * follows:
 *
 * 	System timer base: 0x20003000
 *
 * 	offset		function
 * 	--------------------
 * 	0x00		Control/status
 * 	0x04		Counter (32 low bits)
 * 	0x08		Counter (32 high bits
 * 	0x0C		Compare 0
 * 	0x10		Compare 1
 * 	0x14		Compare 2
 * 	0x18		Compare 3
 *
 * Note that this is NOT the timer used by the ARM Core! Actually it is,
 * but the ARM timer is accessed through the ARM processor at 0x2000B400
 * and is laid out a little differently. If we ever use it, I will add the
 * documentation for it here.
 * 
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ *
 */

#include "timer.h"

/*
 * system timer base address and register offsets
 */
#define TIMER_BASE		0x20003000

#define TIMER_CTRL		0x0
#define TIMER_CNTLO		0x4
#define TIMER_CNTHI		0x8
#define TIMER_CMP0		0xC
#define TIMER_CMP1		0x10
#define TIMER_CMP2		0x14
#define TIMER_CMP3		0x18

/*
 * get the current count
 */
unsigned timer_read()
{
	/* read the low 32 bits from the system clock register */
	return *(unsigned *)(TIMER_BASE + TIMER_CNTLO);
}

/*
 * busy wait
 */
void timer_wait(unsigned ticks)
{
	unsigned start, current;

	/* get the start count */
	start = *(unsigned *)(TIMER_BASE + TIMER_CNTLO);

	/* wait the specified number of ticks */
	do {
		current = *(unsigned *)(TIMER_BASE + TIMER_CNTLO);
	} while (current - start < ticks);
}
