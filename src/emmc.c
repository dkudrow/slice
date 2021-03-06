/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ *
 *
 * src/emmc.c
 *
 * BCM2835 external mass media controller
 *
 * Author:	Daniel Kudrow (dkudrow@cs.ucsb.edu)
 * Date:	March 23 2014
 *
 * Copyright (c) 2014, Daniel Kudrow
 * All rights reserved, see LICENSE.txt for details.
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ *
 *
 * The Raspberry Pi's EMMC implements the MMCA 4.4 and SDHCI 3.0
 * specifications, however this driver only supports SD (mostly because I
 * don't have an mmc card). The driver is exceedingly simple, providing
 * functions to initialize an SD card and perform single block reads and
 * writes.
 *
 * The (SD) host controller must be reset and initialized at start up and
 * each time a card is inserted. We have to:
 *	1. clear the host and card registers
 *	2. configure and enable the clock
 *	3. enable interrupts
 *
 * Once host is initialized, the card must be identified. The ID sequence
 * is:
 *	1. reset the card with the GO_IDLE_STATE command (CMD0)
 *	2. check the card's voltage range with the SD_SEND_IF_COND (CMD8)
 *	3. get the card's OCR register with SD_SEND_OP_COND (ACMD41)
 *	4. send the host's OCR register with SD_SEND_OP_COND (ACMD41)
 *	5. get the card's CID register with ALL_SEND_CID (CMD2)
 *	6. get the card's RCA with SEND_RELATIVE_ADDR (CMD3)
 *	7. select the card with SELECT_CARD (CMD7)
 * 
 * The card is now ready for data transfer operations. The default block
 * length for transfers is 512 bytes.
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ *
 */

#define MODULE EMMC

#include <mailbox.h>
#include <platform.h>
#include <timer.h>
#include <util.h>
#include <log.h>

static volatile emmc_reg_t *emmc_reg = (emmc_reg_t *)EMMC_BASE;

#define IDENT_FREQ	400000		/* clock frequency during initialization */
#define OPER_FREQ	20000000	/* clock frequency during normal operation */
#define BLOCK_SIZE	512			/* block size in bytes */
#define TIMEOUT		100			/* default timeout in ms */
 
/*
 * MMC/SD commands (defined by MMCA 4.4 and SDHCI 3.0)
 */
#define GO_IDLE_STATE		0	/* reset the card to idle state */
#define ALL_SEND_CID		2	/* request CID */
#define SEND_RELATIVE_ADDR	3	/* request RCA */
#define SELECT_CARD			7	/* select a card by RCA */
#define SD_SEND_IF_COND		8	/* get card voltage */
#define SET_BLOCKLEN		16	/* set block length (SDSC only) */
#define READ_SINGLE_BLOCK	17	/* read a single block of data */
#define WRITE_BLOCK			24	/* write a single block of data */

#define APP_CMD				55	/* next command is application specific */
#define	SD_SEND_OP_COND		41	/* get OCR register from SD card */

/*
 * bitmasks for the CMDTM register
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

#define CMD_MASK	0xFF000000	/* mask command index */
#define CMD_SHIFT(x) (x << 24)	/* shift value for command index */

/*
 * bitmasks for the control registers
 */
#define CTRL_RESET_ALL	0x1000000	/* reset complete host circuit */
#define CTRL_RESET_CMD	0x2000000	/* reset command circuit */
#define CTRL_RESET_DAT	0x4000000	/* reset data circuit */
#define CTRL_CLK_MASK	0xFFFF		/* mask all clock control bits */
#define CTRL_INTCLK_EN	0x1			/* enable internal clock */
#define CTRL_STABLE		0x2			/* set when internal clock is stable */
#define CTRL_CLK_EN		0x4			/* enable the clock */
#define CTRL_CLK_GEN	0x20		/* clock generation mode */

#define SHIFT_TIMEOUT(x) (x << 0x10)
#define SHIFT_CLK_GEN(x) ((x & 0xFF) << 0x8 | (x & 0x300) << 0x2)

/*
 * bitmasks for the status register
 */
#define ST_CMD_BUSY		0x1			/* CMD line in user by prev. command */
#define ST_DAT_BUSY		0x2			/* DAT lines in use by prev. transfer */
#define ST_DAT_ACTIVE	0x4			/* at least 1 data line is active */
#define ST_WRITE_RDY	0x100		/* EMMC is ready for a write */
#define ST_READ_RDY		0x200		/* EMMC is ready for a read */
#define ST_CARD_INS		0x10000		/* card is inserted (supported by BCM?) */
#define ST_DAT_LO		0xF00000	/* mask DAT0 - DAT3 */
#define ST_CMD			0x1000000	/* mask CMD */
#define ST_DAT_HI		0x1E000000	/* mask DAT4 - DAT7 */

/*
 * bitmasks for the interrupt registers
 */
#define INT_CMD_DONE	0x1			/* command has finished */
#define INT_DAT_DONE	0x2			/* transfer has finished */
#define INT_BLK_GAP		0x4			/* transfer stopped at block gap */
#define INT_WR_READY	0x10		/* DATA register ready for writing */
#define INT_RD_READY	0x20		/* DATA register ready for reading */
#define INT_CARD		0x100		/* card made interrupt request */
#define INT_RETUNE		0x1000		/* clock retune request */
#define INT_BOOTACK		0x2000		/* boot acknowledge recieved */
#define INT_ENDBOOT		0x4000		/* boot operation has ended */
#define INT_ERROR		0x8000		/* error occured */
#define INT_CTO_ERR		0x10000		/* timeout on CMD */
#define INT_CCRC_ERR	0x20000		/* CRC error on CMD */
#define INT_CEND_ERR	0x40000		/* end bit on CMD not 1 */
#define INT_CBAD_ERR	0x80000		/* wrong command index in response */
#define INT_DTO_ERR		0x100000	/* timeout on DAT */
#define INT_DCRC_ERR	0x200000	/* CRC error on DAT */
#define INT_DEND_ERR	0x400000	/* end bit on DAT not 1 */
#define INT_ACMD_ERR	0x1000000	/* auto command error */
#define INT_MASK_ALL	0x017F7137	/* mask all supported interrupts */

/*
 * OCR fields
 */
#define OCR_VOLTAGE		0xFF8000	/* OCR voltage range */
#define OCR_S18			0x1000000	/* 1 indicates switch to 1.8V mode */
#define OCR_UHSII		0x20000000	/* cars sets to 1 if UHS-II */
#define OCR_CAPACITY	0x40000000	/* card sets to 1 for SDHC/SDXC */
#define OCR_BUSY		0x80000000	/* card sets to 1 when ready */

/*
 * R1 response fields
 */
#define R1_ERRORS		0xFFF90000	/* mask all error bits */

static int capacity;
static int rca;

/*
 * get EMMC clock state from VideoCore
 */
static unsigned emmc_get_clock_state()
{
	unsigned buf[8] __attribute__ ((aligned (16)));

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
		log(ERROR, "bad response from mailbox");
		return -1;
	} else if (buf[6] & 2) {
		log(ERROR, "clock not found");
		return -1;
	} else if (!(buf[6] & 1)) {
		log(ERROR, "clock not on");
		return -1;
	}

	log(DEBUG, "clock is on");

	return 0;
}

/*
 * get EMMC clock rate from VideoCore
 */
static unsigned emmc_get_clock_rate()
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
		log(ERROR, "bad response from mailbox");
		return 0;
	} else if (buf[6] == 0) {
		log(ERROR, "clock not found");
		return 0;
	}

	log(DEBUG, "clock base rate is %u Hz", buf[6]);

	return buf[6];
}

/*
 * busy wait with timeout (in ms)
 */
static int emmc_timeout(volatile uint32_t *reg, unsigned mask, unsigned cond, int timeout)
{
	int i = 0;

	/* loop until the masked register equals the condition */
	while ((*reg & mask) != cond) {
		if (i++ >= timeout)
			return -1;

		/* delay 1 ms */
		timer_wait(1000);
	}

	return i;
}

/*
 * EMMC software reset
 */
static int emmc_host_reset()
{
	unsigned reg;
	int i = 0;

	/* set the software reset bits */
	log(DEBUG, "writing 0x%x to CTRL1 (software reset)", reg);
	emmc_reg->ctrl_1 |= CTRL_RESET_ALL;

	/* host will clear the reset bit when it is done */
	if (emmc_timeout(&emmc_reg->ctrl_1, CTRL_RESET_ALL, 0, TIMEOUT) < 0) {
		log(ERROR, "reset timed out");
		return -1;
	}

	log(INFO, "reset successful");

	return 0;
}

/*
 * set EMMC clock frequency
 */
static int emmc_set_clock(unsigned base, unsigned freq)
{
	unsigned reg, div;
	int i = 0;

	/* turn off the clock */
	reg = emmc_reg->ctrl_1;
	reg &= ~CTRL_CLK_MASK;
	log(DEBUG, "writing 0x%x to CTRL1 (disable clock)", reg);
	emmc_reg->ctrl_1 = reg;

	/* approximate the desired clock frequency */
	if (freq >= base) {
		div = 0;
	} else {
		div = 1;
		/* ARM does not have integer division so... */
		/*while (freq < base / (2 * div))*/
			/*++div;*/
		while (freq < (base >> div))
			++div;
		/* in divided mode freq = base / (2 * div) so add one back to div */
		div = 1 << (div-2);
		log(DEBUG, "frequency divider = %u", div);
	}

	/* set the clock frequency in 'divided clock' mode */
	reg &= ~CTRL_CLK_GEN;
	reg |= SHIFT_CLK_GEN(div);

	/* hardcode the timeout frequency */
	/* TODO: recalculate based on clock */
	reg |= SHIFT_TIMEOUT(0x7);

	/* write clock parameters to EMMC */
	emmc_reg->ctrl_1 = reg;

	/* enable internal clock */
	reg |= CTRL_INTCLK_EN;
	log(DEBUG, "writing 0x%x to CTRL1 (enable internal clock)", reg);
	emmc_reg->ctrl_1 = reg;

	/* host will set the stable bit when the clock is ready */
	if (emmc_timeout(&emmc_reg->ctrl_1, CTRL_STABLE, CTRL_STABLE, TIMEOUT) < 0) {
		log(ERROR, "clock did not stabilize");
		return -1;
	}

	log(DEBUG, "clock is stable");

	/* enable clock on the bus */
	log(DEBUG, "writing 0x%x to CTRL1 (enable bus clock)", reg);
	emmc_reg->ctrl_1 |= CTRL_CLK_EN;

	log(INFO, "clock enabled");

	return 0;
}

/*
 * send command
 */
static int emmc_send_command(unsigned cmd, unsigned arg)
{
	unsigned reg;

	/* wait for CMD line */
	if (emmc_timeout(&emmc_reg->status, ST_CMD_BUSY, 0, TIMEOUT) < 0) {
		log(ERROR, "timed out waiting for CMD line");
		return -1;
	}

	/* TODO: handle busy and abort commands */

	/* wait for DAT line */
	if (emmc_timeout(&emmc_reg->status, ST_DAT_BUSY, 0, TIMEOUT) < 0) {
		log(ERROR, "timed out waiting for DAT line");
		return -1;
	}

	/* set the argument */
	log(DEBUG, "writing 0x%x to ARG1", arg);
	emmc_reg->arg_1 = arg;

	/* prepare and send the command */
	/**(unsigned *)(EMMC_CMDTM) = cmd & ~CMD_MASK;*/
	log(DEBUG, "writing 0x%x to CMDTM", cmd);
	emmc_reg->cmd_tm = cmd;

	/* wait for command done interrupt */
	emmc_timeout(&emmc_reg->interrupt, INT_CMD_DONE, INT_CMD_DONE, TIMEOUT);

	/* TODO: error check */
	if (emmc_reg->interrupt & INT_ERROR) {
		log(ERROR, "error sending command. INTERRUPT: 0x%x", reg);
		return -1;
	}

	log(DEBUG, "command sent successfully");

	/* clear command done interrupt */
	emmc_reg->interrupt = INT_CMD_DONE;

	return 0;
}

/*
 * send application specific command
 */
static int emmc_send_app_command(unsigned acmd, unsigned arg)
{
	unsigned cmd55, arg55;

	/* prepare card for app specific command with APP_CMD */
	cmd55 = CMD_SHIFT(APP_CMD) | CMD_SHORT | CMD_CRC_CK | CMD_I_CK;
	arg55 = 0;
	log(DEBUG, "sending APP_CMD to card");
	if (emmc_send_command(cmd55, arg55) < 0) {
		return -1;
	}

	/* send app specific command */
	log(DEBUG, "sending ACMD to card");
	if (emmc_send_command(acmd, arg) < 0) {
		return -1;
	}

	log(DEBUG, "app specific command sent successfully");

	return 0;
}

/*
 * initialize EMMC host
 */
int emmc_init()
{
	unsigned cmd, arg, reg, resp, base_freq;

	/* get the EMMC clock base rate */
	if (emmc_get_clock_state() != 0)
		return -1;
	base_freq = emmc_get_clock_rate();

	/* reset the EMMC */
	emmc_host_reset();

	/* check whether a card is present */
	if (emmc_timeout(&emmc_reg->status, ST_CARD_INS, ST_CARD_INS, TIMEOUT) < 0) {
		log(ERROR, "did not detect SD card");
		return -1;
	}
	log(INFO, "detected SD card");

	/* set the clock */
	emmc_set_clock(base_freq, IDENT_FREQ);

	/* do not send interrupts to the ARM core */
	emmc_reg->int_enbl = 0;

	/* clear interrupt status register */
	emmc_reg->interrupt = 0xFFFFFFFF;

	/* send interrupts to the INTERRUPT register */
	log(DEBUG, "writing 0x%x to INT_MASK (enable interrupt flags)", reg);
	emmc_reg->int_mask = INT_MASK_ALL;

	/* reset card with */
	arg = 0;
	cmd = CMD_SHIFT(GO_IDLE_STATE);
	log(DEBUG, "sending GO_IDLE_STATE to card");
	if (emmc_send_command(cmd, arg) < 0)
		return -1;

	/* send host voltage range to card */
	arg = 0x1AA;
	cmd = CMD_SHIFT(SD_SEND_IF_COND) | CMD_SHORT | CMD_CRC_CK | CMD_I_CK;
	log(DEBUG, "sending SD_SEND_IF_COND to card");
	if (emmc_send_command(cmd, arg) < 0)
		return -1;

	/* check whether card can run on host's supply voltage */
	resp = emmc_reg->resp_0;
	if (!(resp & 0x100)) {
		log(ERROR, "card voltage not supported. RESP0: 0x%x", resp);
		return -1;
	} else if (resp & 0xFF != 0xAA) {
		log(ERROR, "bad check pattern. Expected 0xAA, found 0x%x", resp);
		return -1;
	}

	/* get card's OCR without initializing card */
	cmd = CMD_SHIFT(SD_SEND_OP_COND) | CMD_SHORT;
	arg = 0;
	log(DEBUG, "sending SD_SEND_OP_COND to card");
	if (emmc_send_app_command(cmd, arg) < 0)
		return -1;

	/* get card capacity from OCR */
	resp = emmc_reg->resp_0;
	if (resp & OCR_BUSY && resp & OCR_CAPACITY)
		capacity = 1;
	else
		capacity = 0;
	log(INFO, "card capacity is %s", capacity ? "SDSC" : "SDHC/SDXC");

	/* initialize card with SD_SENDOP_COND */
	cmd = CMD_SHIFT(SD_SEND_OP_COND) | CMD_SHORT;
	arg = OCR_VOLTAGE | OCR_CAPACITY;
	resp = 0;
	log(DEBUG, "sending SD_SEND_OP_COND to card");
	do {
		if (emmc_send_app_command(cmd, arg) < 0)
			return -1;
		timer_wait(10000);
		resp = emmc_reg->resp_0;
	} while (!(resp & OCR_BUSY));
	log(DEBUG, "card returned 0x%x", resp);

	/* request card's CID register */
	cmd = CMD_SHIFT(ALL_SEND_CID) | CMD_LONG | CMD_CRC_CK;
	arg = 0;
	log(DEBUG, "sending ALL_SEND_CID to card");
	if (emmc_send_command(cmd, arg) < 0)
		return -1;
	
	/* check response to ALL_SEND_CID */
	/* TODO: store CID? */
	resp = emmc_reg->resp_0;
	log(DEBUG, "card returned 0x%x", resp);

	/* request card's RCA */
	cmd = CMD_SHIFT(SEND_RELATIVE_ADDR) | CMD_SHORT | CMD_CRC_CK | CMD_I_CK;
	arg = 0;
	log(DEBUG, "sending SEND_RELATIVE_ADDR to card");
	if (emmc_send_command(cmd, arg) < 0)
		return -1;
	
	/* retrieve RCA */
	resp = emmc_reg->resp_0;
	rca = resp & 0xFFFF0000;
	log(DEBUG, "card returned 0x%x", resp);
	
	/* select card by RCA */
	cmd = CMD_SHIFT(SELECT_CARD) | CMD_BUSY | CMD_CRC_CK | CMD_I_CK;
	arg = rca;
	if (emmc_send_command(cmd, arg) < 0)
		return -1;
	
	/* TODO: check card status */
	resp = emmc_reg->resp_0;
	log(DEBUG, "card returned 0x%x", resp);

	/* set the block length to 512 bytes (only affects SDSC) */
	cmd = CMD_SHIFT(SET_BLOCKLEN) | CMD_SHORT | CMD_CRC_CK | CMD_I_CK;
	arg = BLOCK_SIZE;
	log(DEBUG, "sending SET_BLOCKLEN to card");
	if (emmc_send_command(cmd, arg) < 0)
		return -1;
	
	/* TODO: check response */
	resp = emmc_reg->resp_0;
	log(DEBUG, "card returned 0x%x", resp);

	/* set the clock to operating frequency */
	emmc_set_clock(base_freq, OPER_FREQ);

	log(INFO, "SD card initialized");
	return 0;
}

/*
 * read single block from card
 */
int emmc_read_block(unsigned block, void *void_buf)
{
	unsigned cmd, resp, i;
	unsigned *buf = (unsigned *)void_buf;

	/* set the transfer block size */
	emmc_reg->blksizcnt = BLOCK_SIZE; /* FIXME ??? */

	/* prepare read command with block number as argument */
	cmd = CMD_SHIFT(READ_SINGLE_BLOCK) | TM_DATDIR | CMD_SHORT | CMD_CRC_CK
		| CMD_I_CK | CMD_DATA;
	log(DEBUG, "sending READ_SINGLE_BLOCK to card");
	emmc_send_command(cmd, block);

	/* check response */
	resp = emmc_reg->resp_0;
	if (resp & R1_ERRORS) {
		log(ERROR, "error reading from SD card - bad response: 0x%x", resp);
		return -1;
	}

	/* wait for read ready interrupt */
	if (emmc_timeout(&emmc_reg->interrupt, INT_RD_READY, INT_RD_READY, TIMEOUT) < 0) {
		log(ERROR, "error reading from SD card - timeout waiting for buffer");
		return -1;
	}
	emmc_reg->interrupt = INT_RD_READY;

	/* get data from host */
	for (i=0; i<BLOCK_SIZE; i+=4)
		*buf++ = emmc_reg->data;

	/* wait for transfer complete interrupt */
	if (emmc_timeout(&emmc_reg->interrupt, INT_DAT_DONE, INT_DAT_DONE, TIMEOUT) < 0) {
		log(ERROR, "error reading from SD card - timeout waiting transfer");
		return -1;
	}
	emmc_reg->interrupt = INT_DAT_DONE;

	log(DEBUG, "successfully read block 0x%x", block);

	return 0;
}

/*
 * write single block to card
 */
int emmc_write_block(unsigned block, unsigned *buf)
{
	unsigned cmd, resp, i;

	log(DEBUG, "entering emmc_write_block()");

	/* set the transfer block size */
	emmc_reg->blksizcnt = BLOCK_SIZE;

	/* prepare write command with block number as argument */
	cmd = CMD_SHIFT(WRITE_BLOCK) | CMD_SHORT | CMD_CRC_CK
		| CMD_I_CK | CMD_DATA;
	log(DEBUG, "sending WRITE_BLOCK to card");
	emmc_send_command(cmd, block);

	/* check response */
	resp = emmc_reg->resp_0;
	if (resp & R1_ERRORS) {
		log(ERROR, "error writing to SD card - bad response: 0x%x", resp);
		return -1;
	}

	/* wait for write ready interrupt */
	if (emmc_timeout(&emmc_reg->interrupt, INT_WR_READY, INT_WR_READY, TIMEOUT) < 0) {
		log(ERROR, "error writing to SD card - timeout waiting for buffer");
		return -1;
	}
	emmc_reg->interrupt = INT_WR_READY;

	/* get data from host */
	for (i=0; i<BLOCK_SIZE; i+=4)
		emmc_reg->data = *buf++;

	/* wait for transfer complete interrupt */
	if (emmc_timeout(&emmc_reg->interrupt, INT_DAT_DONE, INT_DAT_DONE, TIMEOUT) < 0) {
		log(ERROR, "error reading from SD card - timeout waiting transfer");
		return -1;
	}
	emmc_reg->interrupt = INT_DAT_DONE;

	log(DEBUG, "successfully wrote to block 0x%x", block);

	return 0;
}

/*
 * print a block
 */
void emmc_dump_block(unsigned char *block)
{
	int i;
	for (i=0; i<BLOCK_SIZE; i++) {
		if (!(i % 32))
			kprintf("\n");
		kprintf("%x", block[i]>>4);
		kprintf("%x", (block[i]) & 0xF);
	}
	kprintf("\n");
}


/*
 * dump all EMMC registers
 */
static void emmc_dump_registers()
{
	kprintf("$>~~~~~ EMMC REGISTER DUMP ~~~~~<$\n");

	kprintf("ARG2: %x, BLKSIZCNT: %x, ARG1: %x, CMDTM: %x\n",
			emmc_reg->arg_2, emmc_reg->blksizcnt,
			emmc_reg->arg_1, emmc_reg->cmd_tm);

	kprintf("RESP0: %x, RESP1: %x, RESP2: %x, RESP3: %x\n",
			emmc_reg->resp_0, emmc_reg->resp_1,
			emmc_reg->resp_2, emmc_reg->resp_3);

	kprintf("DATA: %x, STATUS: %x, CTRL0: %x, CTRL1: %x\n",
			emmc_reg->data, emmc_reg->status,
			emmc_reg->ctrl_0, emmc_reg->ctrl_1);

	kprintf("INT_FLAG: %x, INT_MASK: %x, INT_ENBL: %x\n",
			emmc_reg->interrupt, emmc_reg->int_mask,
			emmc_reg->int_enbl);
}
