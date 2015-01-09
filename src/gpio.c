/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ *
 *
 * src/gpio.c
 *
 * BCM2835 GPIO controller functions
 *
 * Author:	Daniel Kudrow (dkudrow@cs.ucsb.edu)
 * Date:	March 7 2014
 *
 * Copyright (c) 2014, Daniel Kudrow
 * All rights reserved, see LICENSE.txt for details.
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ *
 */

/*
 * GPIO controller base address and register offsets
 */
#define GPIO_BASE		0x20200000

#define GPIO_FSEL		0x0
#define GPIO_SET		0x1C
#define GPIO_CLR		0x28

#define GPIO_MAX_PIN	53
#define GPIO_MAX_FUNC	7

#include <errno.h>
#include <gpio.h>

/*
 * enable a pin in the GPIO controller
 */
int gpio_function_select(unsigned pin, unsigned function)
{
	unsigned bank, offset, reg;

	/* make sure we are accessing a valid pin */
	if (pin > GPIO_MAX_PIN)
		return -EINVAL;

	/* make sure we are setting a valid function */
	if (function > GPIO_MAX_FUNC)
		return -EINVAL;

	/* calculate the bank and offset for the desired pin */
	bank = pin / 10;
	offset = pin - (bank * 10);

	/* read the register from memory */
	reg = *(unsigned *)(GPIO_BASE + GPIO_FSEL + bank);

	/* reset the function */
	reg &= ~(7 << offset);
	reg |= (function << offset);

	/* write back to memory */
	*(unsigned *)(GPIO_BASE + GPIO_FSEL + bank) = reg;

	return 0;
}

/*
 * set a pin
 */
int gpio_set(unsigned pin)
{
	unsigned bank, offset;

	/* make sure we are accessing a valid pin */
	if (pin > GPIO_MAX_PIN)
		return -EINVAL;

	/* calculate the bank and offset for the desired pin */
	bank = pin / 32;
	offset = pin - (bank * 32);

	/* write the correct bit in memory */
	*(unsigned *)(GPIO_BASE + GPIO_SET + bank) = (1 << offset);

	return 0;
}

/*
 * clear a pin
 */
int gpio_clear(unsigned pin)
{
	unsigned bank, offset;

	/* make sure we are accessing a valid pin */
	if (pin > GPIO_MAX_PIN)
		return -EINVAL;

	/* calculate the bank and offset for the desired pin */
	bank = pin / 32;
	offset = pin - (bank * 32);

	/* write the correct bit in memory */
	*(unsigned *)(GPIO_BASE + GPIO_CLR + bank) = (1 << offset);

	return 0;
}
