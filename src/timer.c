/*
 * src/timer.c
 *
 * system timer
 */

#include "timer.h"

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
