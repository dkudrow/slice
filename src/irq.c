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

#define MODULE IRQ

#include <log.h>

/*
 * Interrupt controller base adderss and registers
 *
 * The code below uses the "basic" register as we do not need the full
 * complement of GPU interrupts.
 */
#define IRQ_BASE		0x2000B000

#define IRQ_PENDING		IRQ_BASE+0x200
#define IRQ_ENABLE		IRQ_BASE+0x218
#define IRQ_DISABLE		IRQ_BASE+0x224

/*
 * Interrupt bits for basic register
 */
#define IRQ_ARM_TIMER	0
#define IRQ_MAILBOX		1
#define IRQ_DOORBELL_0	2
#define IRQ_DOORBELL_1	3
#define IRQ_VPU0_HALT	4
#define IRQ_VPU1_HALT	5
#define IRQ_BAD_TYPE0	6
#define IRQ_BAD_TYPE1	7
#define IRQ_PENDING1	8
#define IRQ_PENDING2	9
#define IRQ_JPEG		10
#define IRQ_USB			11
#define IRQ_3D			12
#define IRQ_DMA2		13
#define IRQ_DMA3		14
#define IRQ_I2C			15
#define IRQ_SPI			16
#define IRQ_I2SPCM		17
#define IRQ_SDIO		18
#define IRQ_UART		19
#define IRQ_ARASANSDIO	20

void irq_handle_interrupt()
{
	unsigned reg;

	reg = *(unsigned *)(IRQ_PENDING);

	log(DEBUG, "IRQ_PENDING: 0x%x", reg);
}
