/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ *
 *
 * src/emmc.c
 *
 * BCM2835 external mass media controller
 *
 * Author:	Daniel Kudrow (dkudrow@cs.ucsb.edu)
 * Date:	March 23 2014
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ *
 *
 * The Raspberry Pi's external mass media controller implements the MMCA
 * 4.4 and SDHCI 3.0 specifications but this driver focuses on supporting
 * SD (mostly because I don't have an MMC card).
 *
 * The (SD) host controller must be reset and initialized at start up and
 * each time a card is inserted. We have to:
 * 	1. clear the host and card registers
 * 	2. configure and enable the clock
 * 	3. enable interrupts
 *
 * Once host is initialized, the card must be identified. The ID sequence
 * is:
 * 	1. reset the card with the GO_IDLE_STATE command (CMD0)
 * 	2. check the card's voltage range with the SD_SEND_IF_COND (CMD8)
 * 	3. get the card's OCR register with SD_SEND_OP_COND (ACMD41)
 * 	4. send the host's OCR register with SD_SEND_OP_COND (ACMD41)
 * 	5. get the card's CID register with ALL_SEND_CID (CMD2)
 * 	6. get the card's RCA with SEND_RELATIVE_ADDR (CMD3)
 *	7. select the card with SELECT_CARD (CMD7)
 * 
 * The card is now ready for data transfer operations. The default block
 * length for transfers is 512 bytes.
 *
 */

#include "mailbox.h"
#include "timer.h"
#include "util.h"

#ifdef DEBUG_EMMC
#define PRINT_DEBUG
#endif
#define PRINT_HANDLE "EMMC"
#include "debug.h"

#define IDENT_FREQ 400000	/* clock frequency during initialization */
#define OPER_FREQ 20000000	/* clock frequency during normal operation */
 
/*
 * EMMC base address and registers
 *
 * Although the SDHCI specification defines half-word registers, the
 * BCM2835 can only access word sized registers. This why some registers
 * defined below seem to handle two unrelated tasks. Also bitmasks are fun!
 */
#define EMMC_BASE		0x20300000

#define EMMC_ARG2		EMMC_BASE+0x0
#define EMMC_BLKSIZCNT	EMMC_BASE+0x4
#define EMMC_ARG1		EMMC_BASE+0x8
#define EMMC_CMDTM		EMMC_BASE+0xC
#define EMMC_RESP0		EMMC_BASE+0x10
#define EMMC_RESP1		EMMC_BASE+0x14
#define EMMC_RESP2		EMMC_BASE+0x18
#define EMMC_RESP3		EMMC_BASE+0x1C
#define EMMC_DATA		EMMC_BASE+0x20
#define EMMC_STATUS		EMMC_BASE+0x24	/* current status of the EMMC */
#define EMMC_CTRL0		EMMC_BASE+0x28
#define EMMC_CTRL1		EMMC_BASE+0x2C	/* clock and reset controls for EMMC */
#define EMMC_INTERRUPT	EMMC_BASE+0x30	/* status of EMMC interrupts */
#define EMMC_INT_MASK	EMMC_BASE+0x34	/* event signals INTERRUPT register */
#define EMMC_INT_ENBL	EMMC_BASE+0x38	/* event signals ARM IRQ vector */
#define EMMC_CTRL2		EMMC_BASE+0x3C
#define EMMC_FORCE_INT	EMMC_BASE+0x50
#define EMMC_TIMEOUT	EMMC_BASE+0x70
#define EMMC_DBG		EMMC_BASE+0x74
#define EMMC_FIFO_CFG	EMMC_BASE+0x80
#define EMMC_FIFO_EN	EMMC_BASE+0x84
#define EMMC_TUNE_STEP	EMMC_BASE+0x88
#define EMMC_TUNE_STD	EMMC_BASE+0x8C
#define EMMC_TUNE_DDR	EMMC_BASE+0x90
#define EMMC_INT_SPI	EMMC_BASE+0xF0
#define EMMC_SLOT_VER	EMMC_BASE+0xFC	/* slot status and version info */

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

#define CMD_TIMEOUT			500	/* timeout in ms for commands */

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

#define TIMEOUT_SHIFT	0x10		/* shift value for timeout clock freq. */
#define SHIFT_CLK_GEN(x)\
	((x & 0xFF) << 0x8 | (x & 0x300) << 0x2)

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

static int capacity;
static int rca;

/*
 * busy wait with timeout (in ms)
 */
static int emmc_timeout(unsigned reg, unsigned mask, unsigned cond, int timeout)
{
	int i = 0;

	/* loop until the masked register equals the condition */
	while ((*(unsigned *)reg & mask) != cond) {
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

	debug_print(1, "Entered emmc_host_reset().\n");

	/* set the software reset bits */
	reg = *(unsigned *)(EMMC_CTRL1);
	reg |= CTRL_RESET_ALL;
	debug_print(1, "Writing 0x%x to CTRL1 (software reset).\n", reg);
	*(unsigned *)(EMMC_CTRL1) = reg;

	/* host will clear the reset bit when it is done */
	if (emmc_timeout(EMMC_CTRL1, CTRL_RESET_ALL, 0, 100) < 0) {
		error_print("EMMC reset timed out.\n");
		return -1;
	}

	debug_print(2, "EMMC reset successful.\n");

	return 0;
}

/*
 * set EMMC clock frequency
 */
static int emmc_set_clock(unsigned base, unsigned freq)
{
	unsigned reg, div;
	int i = 0;

	debug_print(1, "Entering emmc_set_clock().\n");

	/* turn off the clock */
	reg = *(unsigned *)(EMMC_CTRL1);
	reg &= ~CTRL_CLK_MASK;
	debug_print(1, "Writing 0x%x to CTRL1 (disable clock).\n", reg);
	*(unsigned *)(EMMC_CTRL1) = reg;

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
		debug_print(1, "Frequency divider = %u.\n", div);
	}

	/* set the clock frequency in 'divided clock' mode */
	reg &= ~CTRL_CLK_GEN;
	reg |= SHIFT_CLK_GEN(div);

	/* hardcode the timeout frequency */
	/* TODO: recalculate based on clock */
	reg |= 0x7 << TIMEOUT_SHIFT;

	/* write clock parameters to EMMC */
	*(unsigned *)(EMMC_CTRL1) = reg;

	/* enable internal clock */
	reg |= CTRL_INTCLK_EN;
	debug_print(1, "Writing 0x%x to CTRL1 (enable internal clock).\n", reg);
	*(unsigned *)(EMMC_CTRL1) = reg;

	/* host will set the stable bit when the clock is ready */
	if (emmc_timeout(EMMC_CTRL1, CTRL_STABLE, CTRL_STABLE, 1000) < 0) {
		error_print("EMMC clock did not stabilize.\n");
		return -1;
	}

	debug_print(1, "EMMC clock is stable.\n");

	/* enable clock on the bus */
	reg = *(unsigned *)(EMMC_CTRL1);
	reg |= CTRL_CLK_EN;
	debug_print(1, "Writing 0x%x to CTRL1 (enable bus clock).\n", reg);
	*(unsigned *)(EMMC_CTRL1) = reg;

	debug_print(2, "EMMC Clock enabled.\n");

	return 0;
}

/*
 * send command
 */
static int emmc_send_command(unsigned cmd, unsigned arg)
{
	unsigned reg;

	debug_print(1, "Entering emmc_send_command().\n");

	/* wait for CMD line */
	if (emmc_timeout(EMMC_STATUS, ST_CMD_BUSY, 0, 100) < 0) {
		error_print("Timed out waiting for CMD line.\n");
		return -1;
	}

	/* TODO: handle busy and abort commands */

	/* wait for DAT line */
	if (emmc_timeout(EMMC_STATUS, ST_DAT_BUSY, 0, 100) < 0) {
		error_print("Timed out waiting for DAT line.\n");
		return -1;
	}

	/* set the argument */
	debug_print(1, "Writing 0x%x to ARG1.\n", arg);
	*(unsigned *)(EMMC_ARG1) = arg;

	/* prepare and send the command */
	/**(unsigned *)(EMMC_CMDTM) = cmd & ~CMD_MASK;*/
	debug_print(1, "Writing 0x%x to CMDTM.\n", cmd);
	*(unsigned *)(EMMC_CMDTM) = cmd;

	/* wait for command done interrupt */
	emmc_timeout(EMMC_INTERRUPT, INT_CMD_DONE, INT_CMD_DONE, 500);

	/* TODO: error check */
	reg = *(unsigned *)(EMMC_INTERRUPT);
	if (reg & INT_ERROR) {
		error_print("Error sending command. INTERRUPT: 0x%x.\n", reg);
		return -1;
	}

	debug_print(1, "EMMC command sent successfully.\n");

	/* clear command done interrupt */
	*(unsigned *)(EMMC_INTERRUPT) = INT_CMD_DONE;

	return 0;
}

/*
 * send application specific command
 */
static int emmc_send_app_command(unsigned acmd, unsigned arg)
{
	unsigned cmd55, arg55;

	debug_print(1, "Entering emmc_send_app_command().\n");

	/* prepare card for app specific command with APP_CMD */
	cmd55 = CMD_SHIFT(APP_CMD) | CMD_SHORT | CMD_CRC_CK | CMD_I_CK;
	arg55 = 0;
	debug_print(1, "Sending APP_CMD to card.\n");
	if (emmc_send_command(cmd55, arg55) < 0) {
		return -1;
	}

	/* send app specific command */
	debug_print(1, "Sending ACMD to card.\n");
	if (emmc_send_command(acmd, arg) < 0) {
		return -1;
	}

	debug_print(1, "EMMC app specific command sent successfully.\n");

	return 0;
}

/*
 * get EMMC clock state from VideoCore
 */
static unsigned emmc_get_clock_state()
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

	debug_print(1, "EMMC clock is on.\n")

	return 0;
}

/*
 * get EMMC clock rate from VideoCore
 */
static unsigned emmc_get_clock_rate()
{
	unsigned buf[8] __attribute__ ((aligned (16)));

	debug_print(1, "Entering emmc_get_clock_rate().\n")

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

	debug_print(1, "EMMC clock base rate is %u Hz.\n", buf[6]);

	return buf[6];
}

/*
 * initialize EMMC host
 */
int emmc_init()
{
	unsigned cmd, arg, reg, resp, base_freq;

	debug_print(1, "Entering emmc_init().\n");

	/* get the EMMC clock base rate */
	if (emmc_get_clock_state() != 0)
		return -1;
	base_freq = emmc_get_clock_rate();

	/* reset the EMMC */
	emmc_host_reset();

	/* check whether a card is present */
	if (emmc_timeout(EMMC_STATUS, ST_CARD_INS, ST_CARD_INS, 100) < 0) {
		error_print("EMMC did not detect SD card.\n");
		return -1;
	}
	debug_print(2, "EMMC detected SD card.\n");

	/* set the clock */
	emmc_set_clock(base_freq, IDENT_FREQ);

	/* do not send interrupts to the ARM core */
	reg = 0;
	*(unsigned *)(EMMC_INT_ENBL) = reg;

	/* clear interrupt status register */
	reg = 0xFFFFFFFF;
	*(unsigned *)(EMMC_INTERRUPT) = reg;

	/* send interrupts to the INTERRUPT register */
	reg = INT_MASK_ALL;
	debug_print(1, "Writing 0x%x to INT_MASK (enable interrupt flags).\n", reg);
	*(unsigned *)(EMMC_INT_MASK) = reg;

	/* send GO_IDLE_STATE to card */
	arg = 0;
	cmd = CMD_SHIFT(GO_IDLE_STATE);
	debug_print(2, "Sending GO_IDLE_STATE to card.\n");
	if (emmc_send_command(cmd, arg) < 0)
		return -1;

	/* send SD_SEND_IF_COND to card */
	arg = 0x1AA;
	cmd = CMD_SHIFT(SD_SEND_IF_COND) | CMD_SHORT | CMD_CRC_CK | CMD_I_CK;
	debug_print(2, "Sending SD_SEND_IF_COND to card.\n");
	if (emmc_send_command(cmd, arg) < 0)
		return -1;

	/* check response to SD_SEND_IF_COND */
	resp = *(unsigned *)(EMMC_RESP0);
	if (!(resp & 0x100)) {
		error_print("Card voltage not supported. RESP0: 0x%x.\n", resp);
		return -1;
	} else if (resp & 0xFF != 0xAA) {
		error_print("Bad check pattern. Expected 0xAA, found 0x%x.\n", resp);
		return -1;
	}

	/* send SD_SEND_OP_COND to card to get card's OCR */
	cmd = CMD_SHIFT(SD_SEND_OP_COND) | CMD_SHORT;
	arg = 0;
	debug_print(2, "Sending SD_SEND_OP_COND to card.\n");
	if (emmc_send_app_command(cmd, arg) < 0)
		return -1;

	/* check response to SD_SEND_OP_COND */
	resp = *(unsigned *)(EMMC_RESP0);
	if (resp & OCR_BUSY && resp & OCR_CAPACITY)
		capacity = 1;
	else
		capacity = 0;
	debug_print(2, "Card capacity is %s.\n", capacity ? "SDSC" : "SDHC/SDXC");

	/* send SD_SEND_OP_COND to card to send host's OCR to card */
	cmd = CMD_SHIFT(SD_SEND_OP_COND) | CMD_SHORT;
	arg = OCR_VOLTAGE | OCR_CAPACITY;
	resp = 0;
	debug_print(2, "Sending SD_SEND_OP_COND to card.\n");
	do {
		if (emmc_send_app_command(cmd, arg) < 0)
			return -1;
		timer_wait(10000);
		resp = *(unsigned *)(EMMC_RESP0);
	} while (!(resp & OCR_BUSY));
	debug_print(2, "Card returned 0x%x.\n", resp);

	/* send ALL_SEND_CID to card */
	cmd = CMD_SHIFT(ALL_SEND_CID) | CMD_LONG | CMD_CRC_CK;
	arg = 0;
	debug_print(2, "Sending ALL_SEND_CID to card.\n");
	if (emmc_send_command(cmd, arg) < 0)
		return -1;
	
	/* check response to ALL_SEND_CID */
	resp = *(unsigned *)(EMMC_RESP0);
	debug_print(2, "Card returned 0x%x.\n", resp);

	/* send SEND_RELATIVE_ADDR to card */
	cmd = CMD_SHIFT(SEND_RELATIVE_ADDR) | CMD_SHORT | CMD_CRC_CK | CMD_I_CK;
	arg = 0;
	debug_print(2, "Sending SEND_RELATIVE_ADDR to card.\n");
	if (emmc_send_command(cmd, arg) < 0)
		return -1;
	
	/* retrieve RCA */
	resp = *(unsigned *)(EMMC_RESP0);
	rca = resp & 0xFFFF0000;
	debug_print(2, "Card returned 0x%x.\n", resp);
	debug_print(2, "Card sent RCA: 0x%x.\n", rca);
	
	/* send SELECT_CARD to card */
	cmd = CMD_SHIFT(SELECT_CARD) | CMD_BUSY | CMD_CRC_CK | CMD_I_CK;
	arg = rca;
	if (emmc_send_command(cmd, arg) < 0)
		return -1;
	
	/* check response to SELECT_CARD */
	resp = *(unsigned *)(EMMC_RESP0);
	debug_print(2, "Card returned 0x%x.\n", resp);
	/* TODO check card status */

	/* set the clock */
	emmc_set_clock(base_freq, OPER_FREQ);

	debug_print(2, "SD card initialized.\n");
	return 0;
}

/*
 * read and write from card
 */
emmc_read()
{
	unsigned cmd, arg;
	
	cmd = CMD_SHIFT(READ_SINGLE_BLOCK) | TM_DATDIR | CMD_SHORT | CMD_CRC_CK
		| CMD_I_CK | CMD_DATA;
}

/*
 * dump all EMMC registers
 */
emmc_dump_registers()
{
	printf("$>~~~~~ EMMC REGISTER DUMP ~~~~~<$\n");

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
			*(unsigned *)(EMMC_INTERRUPT), *(unsigned *)(EMMC_INT_MASK),
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
