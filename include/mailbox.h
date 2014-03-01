/*
 * include/mailbox.h
 *
 * VideoCore mailboxes
 */

#ifndef MAILBOX_H
#define MAILBOX_H

/* regsiter addresses for GPIO controller */
#define MBOX_BASE		0x2000B880

/* offsets for mailbox registers registers */
#define	MBOX_READ		0x0
#define MBOX_POLL		0x10
#define MBOX_SEND		0x14
#define MBOX_STATUS		0x18
#define MBOX_CONF		0x1C
#define	MBOX_WRITE		0x20

#define MBOX_CHAN_PWR	0x0
#define MBOX_CHAN_FB	0x1
#define MBOX_CHAN_USB	0x8
#define MBOX_CHAN_MAX	0x9

/* function prototypes */
int mailbox_write(unsigned channel, unsigned message);
int mailbox_read(unsigned channel, unsigned *message);

#endif /* MAILBOX_H */
