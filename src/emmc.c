/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ *
 *
 * src/emmc.c
 *
 * BCM2835 external mass media controller
 *
 * Author:	Daniel Kudrow (dkudrow@cs.ucsb.edu)
 * Date:	March 7 2014
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ *
 */

#include "debug.h"
#include "mailbox.h"
#include "timer.h"

#define EMMC_CLK_FREQ	50000000
 
/*
 * EMMC base address and registers
 */
#define EMMC_BASE		0x20300000

#define EMMC_ARG2		EMMC_BASE + 0x0
#define EMMC_BLKSIZCNT	EMMC_BASE + 0x4
#define EMMC_ARG1		EMMC_BASE + 0x8
#define EMMC_CMDTM		EMMC_BASE + 0xC
#define EMMC_RESP0		EMMC_BASE + 0x10
#define EMMC_RESP1		EMMC_BASE + 0x14
#define EMMC_RESP2		EMMC_BASE + 0x18
#define EMMC_RESP3		EMMC_BASE + 0x1C
#define EMMC_DATA		EMMC_BASE + 0x20
#define EMMC_STATUS		EMMC_BASE + 0x24
#define EMMC_CTRL0		EMMC_BASE + 0x28
#define EMMC_CTRL1		EMMC_BASE + 0x2C
#define EMMC_INT_FLAG	EMMC_BASE + 0x30
#define EMMC_INT_MASK	EMMC_BASE + 0x34
#define EMMC_INT_ENBL	EMMC_BASE + 0x38
#define EMMC_CTRL2		EMMC_BASE + 0x3C
#define EMMC_FORCE_INT	EMMC_BASE + 0x50
#define EMMC_TIMEOUT	EMMC_BASE + 0x70
#define EMMC_DBG		EMMC_BASE + 0x74
#define EMMC_FIFO_CFG	EMMC_BASE + 0x80
#define EMMC_FIFO_EN	EMMC_BASE + 0x84
#define EMMC_TUNE_STEP	EMMC_BASE + 0x88
#define EMMC_TUNE_STD	EMMC_BASE + 0x8C
#define EMMC_TUNE_DDR	EMMC_BASE + 0x90
#define EMMC_INT_SPI	EMMC_BASE + 0xF0
#define EMMC_SLOT_VER		EMMC_BASE + 0xFC

/*
 * EMMC commands (defined by the MMCA 4.4 specification)
 */
#define GO_IDLE_STATE			0	/* reset the card to idle state */
#define SEND_OP_COND			1	/* get OCR register from card */
#define SEND_EXT_CSD			8	/* send EXT_CSD register */
#define SD_SEND_IF_COND			8
#define SET_BLOCKLEN			16	/* Set the block length (in bytes) */
#define READ_SINGLE_BLOCK		17	/* Read one block */
#define READ_MULTIPLE_BLOCK		18	/* Read multiple blocks */
#define SET_BLOCK_COUNT			23	/* Set no. of blocks to read/write */
#define WRITE_BLOCK				24	/* Write one block */
#define WRITE_MULTIPLE_BLOCK	25	/* write multiple blocks */
#define APP_CMD					55	/* next command is application specific */
#define	SD_APP_OP_COND			41	/* get OCR register from SD card */

/*
 * fields for CMDTM register
 */
#define TM_BLKCNT	0x2			/* enable block counter */
#define TM_AUTOCMD	0xC			/* mask automatic commands */
#define TM_CMD12	0x4			/* send CMD12 upon completion */
#define TM_CMD23	0x8			/* send CMD23 upon completion */
#define TM_DATDIR	0x10		/* 1: card->host, 0: host->card */
#define TM_MULTIBLK	0x20		/* multi-block transfer */
#define CMD_RESP	0x30000		/* expected response type */
#define CMD_NONE	0x0			/* no response */
#define CMD_LONG	0x10000		/* 136 bit response */
#define CMD_SHORT	0x20000		/* 48 bit response */
#define CMD_BUSY	0x30000		/* 48 bit response using busy */
#define CMD_CRC_CK	0x80000		/* set to check response CRC */
#define CMD_I_CK	0x100000	/* set to check response index */
#define CMD_DATA	0x200000	/* command involves transfer */
#define CMD_TYPE	0xC00000	/* command type */
#define CMD_NORMAL	0x0			/* normal command */
#define CMD_SUSPEND	0x400000	/* suspend prev. transfer */
#define CMD_RESUME	0x800000	/* resume prev. transfer */
#define CMD_ABORT	0xC00000	/* abort prev. transfer */
#define CMD_INDEX	0x1F00000	/* command index */

#define CMD_SHIFT(x) (x << 24)	/* shift command to correct possition */

/*
 * Host controls fields
 */
#define CTRL_RESET_ALL	0x1000000	/* reset complete host circuit */
#define CTRL_RESET_CMD	0x2000000	/* reset command circuit */
#define CTRL_RESET_DAT	0x4000000	/* reset data circuit */
#define CTRL_CLK_MASK	0xFFFF		/* mask all clock control bits */
#define CTRL_INTCLK_EN	0x1			/* enable internal clock */
#define CTRL_STABLE		0x2			/* set when internal clock is stable */
#define CTRL_CLK_EN		0x4			/* enable the clock */
#define CTRL_CLK_GEN	0x20		/* clock generation mode */
#define CTRL_GEN_SHIFT	0x8			/* shift value for clock freq. divider */

/*
 * fields for STATUS register
 */
#define ST_CMD_BUSY		0x1			/* CMD line in user by prev. command */
#define ST_DAT_BUSY		0x2			/* DAT lines in use by prev. transfer */
#define ST_DAT_ACTIVE	0x4			/* at least 1 data line is active */
#define ST_WRITE_RDY	0x100		/* ST is ready for a write */
#define ST_READ_RDY		0x200		/* ST is ready for a read */
#define ST_DAT_LO		0xF00000	/* mask DAT0 - DAT3 */
#define ST_CMD			0x1000000	/* mask CMD */
#define ST_DAT_HI		0x1E000000	/* mask DAT4 - DAT7 */

struct emmc_status_t {
	unsigned cmd_busy;
	unsigned dat_busy;
	unsigned dat_active;
	unsigned write_rdy;
	unsigned read_rdy;
	unsigned dat_lo;
	unsigned cmd;
	unsigned dat_hi;
};

/*
 * reset host
 */
static int emmc_host_reset()
{
	unsigned reg;
	int i = 0;

	debug_print(1, "Entered emmc_host_reset().\n");

	/* set the software reset bits */
	reg = *(unsigned *)(EMMC_CTRL1);
	reg |= CTRL_RESET_ALL;
	debug_print(1, "Writing 0x%x to CTRL1 (software reset).\n", reg);
	*(unsigned *)(EMMC_CTRL1) = reg;

	/* host will clear the reset bit when it is done */
	while (*(unsigned *)(EMMC_CTRL1) & CTRL_RESET_ALL) {
		/* delay 1 ms */
		timer_wait(1000);

		/* timeout after 100 ms */
		if (i++ >= 100) {
			error_print("EMMC reset timed out.\n");
			return -1;
		}
	}

	return 0;
}

/*
 * set clock
 */
static int emmc_set_clock()
{
	unsigned reg;
	int i = 0;

	debug_print(1, "Entering emmc_set_clock().\n");

	/* turn off the clock */
	reg = *(unsigned *)(EMMC_CTRL1);
	reg &= ~CTRL_CLK_MASK;
	debug_print(1, "Writing 0x%x to CTRL1 (disable clock).\n", reg);
	*(unsigned *)(EMMC_CTRL1) = reg;

	/* set the clock frequency in 'divided clock' mode */
	reg &= ~CTRL_CLK_GEN;
	/* freq. is MAX / N */
	reg |= (0x5 << CTRL_GEN_SHIFT);

	/* write parameters */
	*(unsigned *)(EMMC_CTRL1) = reg;

	/* enable clock */
	reg |= CTRL_CLK_EN;
	debug_print(1, "Writing 0x%x to CTRL1 (enable clock).\n", reg);
	*(unsigned *)(EMMC_CTRL1) = reg;

	/* host will set the stable bit when the clock is ready */
	while (!((*(unsigned *)(EMMC_CTRL1)) & CTRL_STABLE)) {
		/* timeout after 100 ms */
		if (i++ >= 100) {
			error_print("EMMC clock did not stabilize.\n");
			return -1;
		}

		timer_wait(1000);
	}

	return 0;
}

/*
 * get EMMC clock state from VideoCore
 */
unsigned emmc_get_clock_state()
{
	unsigned buf[8] __attribute__ ((aligned (16)));

	debug_print(1, "Entering emmc_get_clock_state().\n")

	/* property tag buffer */
	buf[0] = 32;		/* size of buffer */
	buf[1] = 0;			/* this is a request buffer */
	/* begin tag */
	buf[2] = 0x30001;	/* request clock state */
	buf[3] = 8;			/* total size of value buffer */
	buf[4] = 4;			/* size of request value buffer */
	/* value buffer */
	buf[5] = 1;			/* EMMC clock ID */
	buf[6] = 0;			/* leave room for response */
	/* end tag */
	buf[7] = 0;	

	mailbox_write(MBOX_CHAN_PROP, (unsigned)buf);
	mailbox_read(MBOX_CHAN_PROP, buf);

	if (buf[1] != MBOX_PROP_OK) {
		error_print("Bad response from mailbox.\n");
		return -1;
	} else if (buf[6] & 2) {
		error_print("EMMC clock not found.\n");
		return -1;
	} else if (!(buf[6] & 1)) {
		error_print("EMMC clock not on.\n");
		return -1;
	}

	return 0;
}

/*
 * get EMMC clock rate from VideoCore
 */
unsigned emmc_get_clock_rate()
{
	unsigned buf[8] __attribute__ ((aligned (16)));

	/* property tag buffer */
	buf[0] = 32;		/* size of buffer */
	buf[1] = 0;			/* this is a request buffer */
	/* begin tag */
	buf[2] = 0x30002;	/* request clock state */
	buf[3] = 8;			/* total size of value buffer */
	buf[4] = 4;			/* size of request value buffer */
	/* value buffer */
	buf[5] = 1;			/* EMMC clock ID */
	buf[6] = 0;			/* leave room for response */
	/* end tag */
	buf[7] = 0;	

	mailbox_write(MBOX_CHAN_PROP, (unsigned)buf);
	mailbox_read(MBOX_CHAN_PROP, buf);

	if (buf[1] != MBOX_PROP_OK) {
		error_print("Bad response from mailbox.\n");
		return 0;
	} else if (buf[6] == 0) {
		error_print("EMMC clock not found.\n");
		return 0;
	}

	debug_print(1, "EMMC clock rate is %u Hz.\n", buf[6]);

	return buf[6];
}

/*
 * initialize EMMC host
 */
int emmc_init()
{
	unsigned max_clock;

	debug_print(1, "Entering emmc_init().\n");

	/* get the EMMC clock base rate */
	if (emmc_get_clock_state() != 0)
		return -1;
	max_clock = emmc_get_clock_rate();

	emmc_host_reset();
	emmc_dump_registers();
	emmc_set_clock();
	emmc_dump_registers();
}

/*
 * read STATUS register
 */
unsigned emmc_get_status(struct emmc_status_t *status)
{
	unsigned reg = *(unsigned *)(EMMC_BASE + EMMC_STATUS);

	printf("STATUS: 0x%x\n", reg);

	if (status) {
		status->cmd_busy = reg & ST_CMD_BUSY;
		status->dat_busy = reg & ST_DAT_BUSY;
		status->dat_active = reg & ST_DAT_ACTIVE;
		status->write_rdy = reg & ST_WRITE_RDY;
		status->read_rdy = reg & ST_READ_RDY;
		status->dat_lo = (reg & ST_DAT_LO) >> 20;
		status->cmd = (reg & ST_CMD) >> 24;
		status->dat_hi = (reg & ST_DAT_HI) >> 25;
	}

	return reg;
}

/*
 * dump the contents of the STATUS register
 */
void emmc_dump_status()
{
	struct emmc_status_t status;

	emmc_get_status(&status);

	printf("CMD: %x, DAT[0:3]: %x, DAT[4:7]: %x\n", status.cmd, status.dat_lo, status.dat_hi);
	printf("write ready: %u, read ready: %u\n", status.write_rdy, status.read_rdy);
	printf("cmd busy: %u, dat busy: %u, dat active: %u\n", status.cmd_busy, status.dat_busy, status.dat_active);
}

/*
 * dump registers
 */
emmc_dump_registers()
{
	printf("$>===== EMMC REGISTER DUMP =====<$\n");

	printf("ARG2: %x, BLKSIZCNT: %x, ARG1: %x, CMDTM: %x\n",
			*(unsigned *)(EMMC_ARG2), *(unsigned *)(EMMC_BLKSIZCNT),
			*(unsigned *)(EMMC_ARG1), *(unsigned *)(EMMC_CMDTM));

	printf("RESP0: %x, RESP1: %x, RESP2: %x, RESP3: %x\n",
			*(unsigned *)(EMMC_RESP0), *(unsigned *)(EMMC_RESP1),
			*(unsigned *)(EMMC_RESP2), *(unsigned *)(EMMC_RESP3));

	printf("DATA: %x, STATUS: %x, CTRL0: %x, CTRL1: %x\n",
			*(unsigned *)(EMMC_DATA), *(unsigned *)(EMMC_STATUS),
			*(unsigned *)(EMMC_CTRL0), *(unsigned *)(EMMC_CTRL1));

	printf("INT_FLAG: %x, INT_MASK: %x, INT_ENBL: %x, CTRL2: %x\n",
			*(unsigned *)(EMMC_INT_FLAG), *(unsigned *)(EMMC_INT_MASK),
			*(unsigned *)(EMMC_INT_ENBL), *(unsigned *)(EMMC_CTRL2));

	printf("FORCE_INT: %x, TIMEOUT: %x, DBG: %x, FIFO_CFG: %x\n",
			*(unsigned *)(EMMC_FORCE_INT), *(unsigned *)(EMMC_TIMEOUT),
			*(unsigned *)(EMMC_DBG), *(unsigned *)(EMMC_FIFO_CFG));

	printf("FIFO_EN: %x, TUNE_STEP: %x, TUNE_STD: %x, TUNE_DDR: %x\n",
			*(unsigned *)(EMMC_FIFO_EN), *(unsigned *)(EMMC_TUNE_STEP),
			*(unsigned *)(EMMC_TUNE_STD), *(unsigned *)(EMMC_TUNE_DDR));

	printf("INT_SPI: %x, SLOTISR: %x\n",
			*(unsigned *)(EMMC_INT_SPI), *(unsigned *)(EMMC_SLOT_VER));
}
