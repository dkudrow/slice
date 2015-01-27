/* include/platform.h -- TODO */

#ifndef PLATFORM_H
#define PLATFORM_H

#include <types.h>

/* BCM2835 memory mapped peripherals */
#define PERIPHERAL_BASE		0x20000000

/* Timer */
#define TIMER_BASE		PERIPHERAL_BASE + 0x3000

typedef struct __attribute__((packed)) {
	uint32_t status;	/* 0x00 */
	uint32_t count_lo;	/* 0x04 */
	uint32_t count_hi;	/* 0x08 */
	uint32_t cmp_0;		/* 0x0C */
	uint32_t cmp_1;		/* 0x10 */
	uint32_t cmp_2;		/* 0x14 */
	uint32_t cmp_3;		/* 0x18 */
} timer_reg_t;

/* Interrupt controller */
#define IRQ_BASE		PERIPHERAL_BASE + 0xB200

typedef struct __attribute__((packed)) {
	uint32_t basic_pending;
	uint32_t pending_1;
	uint32_t pending_2;
	uint32_t fiq_ctrl;
	uint32_t enable_1;
	uint32_t enable_2;
	uint32_t basic_enable;
	uint32_t disable_1;
	uint32_t disable_2;
	uint32_t basic_disable;
} irq_reg_t;

/* VideoCore mailbox */
#define MBOX_BASE		PERIPHERAL_BASE + 0xB880

typedef struct __attribute__((packed)) {
	uint32_t read;		/* 0x00 */
	uint32_t PAD[3];
	uint32_t poll;		/* 0x10 */
	uint32_t send;		/* 0x14 */
	uint32_t status;	/* 0x18 */
	uint32_t config;	/* 0x1C */
	uint32_t write;	/* 0x20 */
} mailbox_reg_t;

/* General purpose I/O controller */
#define GPIO_BASE		PERIPHERAL_BASE + 0x200000

typedef struct __attribute__((packed)) {
	uint32_t func_select[6];	/* 0x00 */
	uint32_t PAD1;
	uint32_t set[2];		/* 0x1C */
	uint32_t PAD2;
	uint32_t clear[2];		/* 0x28 */
} gpio_reg_t;

/* External mass media controller */
#define EMMC_BASE		PERIPHERAL_BASE + 0x300000

typedef struct __attribute__((packed)) {
	uint32_t arg_2;		/* 0x00 */
	uint32_t blksizcnt;	/* 0x04 - transfer block size, count */
	uint32_t arg_1;		/* 0x08 - command argument */
	uint32_t cmd_tm;	/* 0x0C - command and transfer mode */
	uint32_t resp_0;	/* 0x10 - first word of response */
	uint32_t resp_1;	/* 0x14 - second word of response */
	uint32_t resp_2;	/* 0x18 - third word of response */
	uint32_t resp_3;	/* 0x1C - fourth word of response */
	uint32_t data;		/* 0x20 - next word of EMMC buffer */
	uint32_t status;	/* 0x24 - current status of the EMMC */
	uint32_t ctrl_0;	/* 0x28 - DAT line, boot mode control */
	uint32_t ctrl_1;	/* 0x2C - EMMC clock, reset control */
	uint32_t interrupt;	/* 0x30 - status of EMMC interrupts */
	uint32_t int_mask;	/* 0x34 - event signals INTERRUPT */
	uint32_t int_enbl;	/* 0x38 - event signals ARM IRQ */
} emmc_reg_t;

#endif /* PLATFORM_H */

