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

#include <errno.h>
#include <gpio.h>
#include <platform.h>
#include <util.h>

static volatile gpio_reg_t *gpio_reg = (gpio_reg_t *)GPIO_BASE;

#define GPIO_MAX_PIN	53
#define GPIO_MAX_FUNC	7

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
	/*reg = READ4(GPIO_FSEL + bank);*/
	reg = gpio_reg->func_select[bank];

	/* reset the function */
	reg &= ~(7 << offset);
	reg |= (function << offset);

	/* write back to memory */
	/*WRITE4(GPIO_FSEL + bank, reg);*/
	gpio_reg->func_select[bank] = reg;

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
	/*WRITE4(GPIO_SET + bank, 1 << offset);*/
	gpio_reg->set[bank] = 1 << offset;

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
	/*WRITE4(GPIO_CLR + bank, 1 << offset);*/
	gpio_reg->clear[bank] = 1 << offset;

	return 0;
}
