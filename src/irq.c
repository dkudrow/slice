/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ *
 *
 * src/irq.c
 *
 * IRQ service routine
 *
 * Author:	Daniel Kudrow (dkudrow@cs.ucsb.edu)
 * Date:	April 19 2014
 *
 * Copyright (c) 2014, Daniel Kudrow
 * All rights reserved, see LICENSE.txt for details.
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ *
 */

#ifdef DEBUG_IRQ
#define PRINT_DEBUG
#endif
#define PRINT_HANDLE "IRQ"
#include "debug.h"

/*
 * BCM2835 Interrupt controller base adderss and registers
 *
 * The code below uses the "basic" register as we do not need the full
 * complement of GPU interrupts.
 */
#define IRQ_BASE		0x2000B000

#define IRQ_PENDING		IRQ_BASE+0x200
#define IRQ_ENABLE		IRQ_BASE+0x218
#define IRQ_DISABLE		IRQ_BASE+0x224

void irq_handle_interrupt()
{
	unsigned reg;

	reg = *(unsigned *)(IRQ_PENDING);

	debug_print("IRQ_PENDING: 0x%x\n", reg);
}
