/*
 * src/mailbox.c
 *
 * VideoCore mailboxes
 */

#include "errno.h"
#include "mailbox.h"

/*
 * write a message to the VideoCore mailbox
 * note: message data is *not* shifted!
 */
int mailbox_write(unsigned channel, unsigned message)
{
	unsigned reg;

	/* validate inputs */
	if (channel > MBOX_CHAN_MAX)
		return -EINVAL;
	if (message & 0xF)
		return -EINVAL;

	/* message takes upper 28 bits and channel takes lower 4 bits */
	reg = (message & ~0xF) | (channel & 0xF);

	/* write message to mailbox */
	/* wait for the status register to clear for writing */
	/* TODO this shouldn't be a bsy wait */
	while (*(unsigned *)(MBOX_BASE + MBOX_STATUS) & (1 << 31))
		;

	*(unsigned *)(MBOX_BASE + MBOX_WRITE) = reg;

	return 0;
}

/*
 * read a message from the VideoCore mailbox
 * note: message data is *not* shifted!
 */
int mailbox_read(unsigned channel, unsigned *message)
{
	unsigned reg;

	/* validate input */
	if (channel > MBOX_CHAN_MAX)
		return -EINVAL;

	/* loop until we are on the right channel */
	do {
		/* wait for the status register to clear for reading */
		while (*(unsigned *)(MBOX_BASE + MBOX_STATUS) & (1 << 30))
			;

		/* read message to mailbox */
		reg = *(unsigned *)(MBOX_BASE + MBOX_READ);
	} while (channel != (reg & 0xF));

	/* save message */
	*message = reg & ~0xF;

	return 0;
}
