/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ *
 *
 * include/gpio.h
 *
 * BCM2835 GPIO controller functions
 *
 * Author:	Daniel Kudrow (dkudrow@cs.ucsb.edu)
 * Date:	March 7 2014
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ *
 */ 

#ifndef GPIO_H
#define GPIO_H

/* function prototypes */
int gpio_function_select(unsigned pin, unsigned function);
int gpio_set(unsigned pin);
int gpio_clear(unsigned pin);

#endif /* GPIO_H */
