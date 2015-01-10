/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ *
 *
 * include/gpio.h
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

#ifndef GPIO_H
#define GPIO_H

/* Function prototypes */
int gpio_function_select(unsigned pin, unsigned function);
int gpio_set(unsigned pin);
int gpio_clear(unsigned pin);

#endif /* GPIO_H */
