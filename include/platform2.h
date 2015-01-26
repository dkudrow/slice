/* include/platform.h -- TODO */

#ifndef INCLUDE/PLATFORM_H
#define INCLUDE/PLATFORM_H

/* BCM2835 memory mapped peripherals */
#define PERIPHERAL_BASE		0x20000000

/* Timer */

#define TIMER_BASE		PERIPHERAL_BASE + 0x3000

typedef struct __attribute__ ((packed)) {
	volatile uint32_t status;	/* 0x00 */
	volatile uint32_t count_hi;	/* 0x04 */
	volatile uint32_t count_lo;	/* 0x08 */
	volatile uint32_t cmp_0;	/* 0x0C */
	volatile uint32_t cmp_1;	/* 0x10 */
	volatile uint32_t cmp_2;	/* 0x14 */
	volatile uint32_t cmp_3;	/* 0x18 */
} timer_reg_t;

/* VideoCore mailbox */
#define MBOX_BASE		PERIPHERAL_BASE + 0xB880

typdef struct __attribute__((packed)) {
	volatile uint32_t read;		/* 0x00 */
	uint32_t PAD[3];
	volatile uint32_t poll;		/* 0x10 */
	volatile uint32_t send;		/* 0x14 */
	volatile uint32_t status;	/* 0x18 */
	volatile uint32_t config;	/* 0x1C */
	volatile uint32_t write;	/* 0x20 */
} mailbox_reg_t;

/* General purpose I/O controller */
#define GPIO_BASE		PERIPHERAL_BASE + 0x200000

typdef struct __attribute__((packed)) {
	volatile uint32_t func_select;	/* 0x00 */
	uint32_t PAD1[6];
	volatile uint32_t set;		/* 0x1C */
	uint32_t PAD2[2];
	volatile uint32_t clear;	/* 0x28 */
} gpio_reg_t;

/* External mass media controller */
#define EMMC_BASE		PERIPHERAL_BASE + 0x300000

typedef struct __attribute__((packed)) {
	volatile uint32_t arg_2;	/* 0x00 */
	volatile uint32_t blksizcnt;	/* 0x04 - transfer block size, count */
	volatile uint32_t arg_1;	/* 0x08 - command argument */
	volatile uint32_t cmd_tm;	/* 0x0C - command and transfer mode */
	volatile uint32_t resp_0;	/* 0x10 - first word of response */
	volatile uint32_t resp_1;	/* 0x14 - second word of response */
	volatile uint32_t resp_2;	/* 0x18 - third word of response */
	volatile uint32_t resp_3;	/* 0x1C - fourth word of response */
	volatile uint32_t data;		/* 0x20 - next word of EMMC buffer */
	volatile uint32_t status;	/* 0x24 - current status of the EMMC */
	volatile uint32_t ctrl_0;	/* 0x28 - DAT line, boot mode control */
	volatile uint32_t ctrl_1;	/* 0x2C - EMMC clock, reset control */
	volatile uint32_t interrupt;	/* 0x30 - status of EMMC interrupts */
	volatile uint32_t int_mask;	/* 0x34 - event signals INTERRUPT */
	volatile uint32_t int_enbl;	/* 0x38 - event signals ARM IRQ */
} emmc_reg_t;

#endif /* INCLUDE/PLATFORM_H */

