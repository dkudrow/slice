/* include/platform.h -- TODO */

#ifndef PLATFORM_H
#define PLATFORM_H

/* BCM2835 memory mapped peripherals */
#define PERIPHERAL_BASE		0x20000000

/* Timer */
#define TIMER_BASE		PERIPHERAL_BASE + 0x3000

#define TIMER_CTRL		TIMER_BASE + 0x0
#define TIMER_CNTLO		TIMER_BASE + 0x4
#define TIMER_CNTHI		TIMER_BASE + 0x8
#define TIMER_CMP0		TIMER_BASE + 0xC
#define TIMER_CMP1		TIMER_BASE + 0x10
#define TIMER_CMP2		TIMER_BASE + 0x14
#define TIMER_CMP3		TIMER_BASE + 0x18


/* VideoCore mailbox */
#define MBOX_BASE		PERIPHERAL_BASE + 0xB880

#define	MBOX_READ		MBOX_BASE + 0x0
#define MBOX_POLL		MBOX_BASE + 0x10
#define MBOX_SEND		MBOX_BASE + 0x14
#define MBOX_STATUS		MBOX_BASE + 0x18
#define MBOX_CONF		MBOX_BASE + 0x1C
#define	MBOX_WRITE		MBOX_BASE + 0x20

/* General purpose I/O controller */
#define GPIO_BASE		PERIPHERAL_BASE + 0x200000

#define GPIO_FSEL		GPIO_BASE + 0x0
#define GPIO_SET		GPIO_BASE + 0x1C
#define GPIO_CLR		GPIO_BASE + 0x28

/* External mass media controller */
#define EMMC_BASE		PERIPHERAL_BASE + 0x300000

#define EMMC_ARG2	EMMC_BASE + 0x0
#define EMMC_BLKSIZCNT	EMMC_BASE + 0x4		/* block size and count for transfer */
#define EMMC_ARG1	EMMC_BASE + 0x8		/* command argument */
#define EMMC_CMDTM	EMMC_BASE + 0xC		/* command and transfer mode */
#define EMMC_RESP0	EMMC_BASE + 0x10	/* first word of response */
#define EMMC_RESP1	EMMC_BASE + 0x14	/* second word of response */
#define EMMC_RESP2	EMMC_BASE + 0x18	/* third word of response */
#define EMMC_RESP3	EMMC_BASE + 0x1C	/* fourth word of response */
#define EMMC_DATA	EMMC_BASE + 0x20	/* next word of EMMC buffer */
#define EMMC_STATUS	EMMC_BASE + 0x24	/* current status of the EMMC */
#define EMMC_CTRL0	EMMC_BASE + 0x28	/* DAT line and boot mode control */
#define EMMC_CTRL1	EMMC_BASE + 0x2C	/* clock and reset controls for EMMC */
#define EMMC_INTERRUPT	EMMC_BASE + 0x30	/* status of EMMC interrupts */
#define EMMC_INT_MASK	EMMC_BASE + 0x34	/* event signals INTERRUPT register */
#define EMMC_INT_ENBL	EMMC_BASE + 0x38	/* event signals ARM IRQ vector */
#define EMMC_CTRL2	EMMC_BASE + 0x3C
#define EMMC_FORCE_INT	EMMC_BASE + 0x50
#define EMMC_TIMEOUT	EMMC_BASE + 0x70
#define EMMC_DBG	EMMC_BASE + 0x74
#define EMMC_FIFO_CFG	EMMC_BASE + 0x80
#define EMMC_FIFO_EN	EMMC_BASE + 0x84
#define EMMC_TUNE_STEP	EMMC_BASE + 0x88
#define EMMC_TUNE_STD	EMMC_BASE + 0x8C
#define EMMC_TUNE_DDR	EMMC_BASE + 0x90
#define EMMC_INT_SPI	EMMC_BASE + 0xF0
#define EMMC_SLOT_VER	EMMC_BASE + 0xFC	/* slot status and version info */

#endif /* PLATFORM_H */

