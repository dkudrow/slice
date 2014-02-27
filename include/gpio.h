/*
 * include/gpio.h
 *
 * macros and functions for the GPIO controller
 */ 

#ifndef GPIO_H
#define GPIO_H

/* regsiter addresses for GPIO controller */
#define GPIO_BASE		0x20200000

/* offsets for GPIO registers */
#define GPIO_FSEL		0x0
#define GPIO_SET		0x1C
#define GPIO_CLR		0x28

#define GPIO_MAX_PIN 53
#define GPIO_MAX_FUNCTION 7

/* function prototypes */
int gpio_function_select(unsigned pin, unsigned function);
int gpio_set(unsigned pin);
int gpio_clear(unsigned pin);

#endif /* GPIO_H */
