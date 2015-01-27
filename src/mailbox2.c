/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ *
 *
 * src/mailbox.c
 *
 * BCM2835 VideoCore mailboxes
 *
 * Author:	Daniel Kudrow (dkudrow@cs.ucsb.edu)
 * Date:	March 7 2014
 *
 * Copyright (c) 2014, Daniel Kudrow
 * All rights reserved, see LICENSE.txt for details.
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ *
 *
 * The mailbox is the main point of communication between the ARM Core and
 * the VideoCore. Before I go on, mailboxes are covered extensively here:
 * https://github.com/raspberrypi/firmware/wiki/ so for a more thorough
 * treatment look there. The mailbox contains channels that allow the ARM
 * Core to communicate to different elements within the VC. The mailbox is
 * laid out as follows:
 *
 *	Mailbox base: 0x2000B880
 *
 *	offset		function
 *	---------------------
 *	0x00		Read
 *	0x10		Peek
 *	0x14		Sender
 *	0x18		Status
 *	0x1C		Config
 *	0x20		Write
 *
 * Messages are 32 bits arranged as follows:
 *
 *	---------------------------
 *	| 31:4 DATA | 3:0 CHANNEL |
 *	---------------------------
 *
 * Be wary when writing addresses to the mailbox - the bottom 4 bits will
 * be clobbered by the channel so make certain that the address is aligned
 * to a 16-byte boundary. Also, addresses are read by the VC's MMU so
 * physical addresses must start at 0x40000000 (L2 caching enabled) or
 * 0x80000000 (L2 caching disabled). Likewise, addresses returned by the
 * VC must be translated back into ARM physical addresses by subtracting
 * 0x40000000 (or 0x80000000 as the case may be).
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ *
 */

#include <errno.h>
#include <mailbox.h>
#include <platform2.h>
#include <util.h>

static volatile mailbox_reg_t *mailbox = MBOX_BASE;

#define WRITE_READY (1 << 31)
#define READ_READY (1 << 30)

/*
 * Write a message to the VideoCore mailbox
 * note: message data is *not* shifted!
 */
int mailbox_write(int channel, uint32_t message)
{
	uint32_t reg;

	/* validate inputs */
	if (channel > MBOX_CHAN_MAX)
		return -EINVAL;
	if (message & 0xF)
		return -EINVAL;

	/* message takes upper 28 bits and channel takes lower 4 bits */
	reg = (message & ~0xF) | (channel & 0xF);

	/* wait for the status register to clear for writing */
	/*while (READ4(MBOX_STATUS) & WRITE_READY)*/
	while (mailbox->status & WRITE_READY)
		;

	/* write message to mailbox */
	/*WRITE4(MBOX_WRITE, reg);*/
	mailbox->write = reg;

	return 0;
}

/*
 * Read a message from the VideoCore mailbox
 * note: message data is *not* shifted!
 */
int mailbox_read(int channel, uint32_t *message)
{
	uint32_t reg;

	/* validate input */
	if (channel > MBOX_CHAN_MAX)
		return -EINVAL;

	/* loop until we are on the right channel */
	do {
		/* wait for the status register to clear for reading */
		/*while (READ4(MBOX_STATUS) & READ_READY)*/
		while (mailbox->status & READ_READY)
			;

		/* read message to mailbox */
		/*reg = READ4(MBOX_READ);*/
		reg = mailbox->read;
	} while (channel != (reg & 0xF));

	/* save message */
	*message = reg & ~0xF;

	return 0;
}
