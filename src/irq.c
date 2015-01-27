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

#include <irq.h>
#include <log.h>
#include <platform.h>

static volatile irq_reg_t *irq = IRQ_BASE;

static irq_handler_t irq_handlers[96];

void irq_register_handler(irq_handler_t handler, int pos)
{
	irq_handlers[pos] = handler;
}

static void irq_handle_pending(int bank)
{
	/* TODO */
}

void irq_handle_bank_1_pending(void)
{
	irq_handle_pending(1);
}

void irq_handle_bank_2_pending(void)
{
	irq_handle_pending(2);
}

void irq_init()
{
	irq_register_handler(irq_handle_bank_1_pending, 8);
	irq_register_handler(irq_handle_bank_2_pending, 9);
}

/*
 * TODO: explain bcm2835 irq registers
 */
void service_irq()
{
	int bit;
	uint32_t basic;

	log(DEBUG, "basic_pend: 0x%x, pend_1: 0x%x, pend_2: 0x%x", irq->basic_pending, irq->pending_1, irq->pending_2);
	log(DEBUG, "basic_enbl: 0x%x, enbl_1: 0x%x, enbl_2: 0x%x", irq->basic_enable, irq->enable_1, irq->enable_2);
	log(DEBUG, "basic_dabl: 0x%x, dabl_1: 0x%x, dabl_2: 0x%x", irq->basic_disable, irq->disable_1, irq->disable_2);

	basic = irq->basic_pending;
	while (basic) {
		bit = 31 - __builtin_clz(basic);
		(irq_handlers[bit])();
		basic ^= (1 << bit); 
	}
}

void service_undefined()
{
	log(WARN, "handling undefined");
}

void service_software_int()
{
	log(WARN, "handling software_int");
}

void service_prefetch_abort()
{
	log(WARN, "handling prefetch_abort");
}

void service_data_abort()
{
	log(WARN, "handling data_abort");
}

void service_fiq()
{
	log(WARN, "handling fiq");
}
