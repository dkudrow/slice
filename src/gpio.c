/*
 * src/gpio.c
 *
 * GPIO controller functions
 */

#include "errno.h"
#include "gpio.h"

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
	if (function > GPIO_MAX_FUNCTION)
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
