/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ *
 *
 * include/mailbox.h
 *
 * VideoCore mailboxes
 *
 * Author:	Daniel Kudrow (dkudrow@cs.ucsb.edu)
 * Date:	March 7 2014
 *
 * Copyright (c) 2014, Daniel Kudrow
 * All rights reserved, see LICENSE.txt for details.
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ *
 */

#ifndef MAILBOX_H
#define MAILBOX_H

/*
 * mailbox channels
 */
#define MBOX_CHAN_PWR	0x0
#define MBOX_CHAN_FB	0x1
#define MBOX_CHAN_PROP	0x8
#define MBOX_CHAN_MAX	0x9

#define MBOX_PROP_OK	0x80000000
#define MBOX_PROP_ERR	0x80000001

/*
 * function prototypes
 */
int mailbox_write(unsigned channel, unsigned message);
int mailbox_read(unsigned channel, unsigned *message);

#endif /* MAILBOX_H */
