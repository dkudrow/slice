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

#include <types.h>

/* Mailbox channels */
#define MBOX_CHAN_PWR	0x0
#define MBOX_CHAN_FB	0x1
#define MBOX_CHAN_PROP	0x8
#define MBOX_CHAN_MAX	0x9

#define MBOX_PROP_OK	0x80000000
#define MBOX_PROP_ERR	0x80000001

/* Function prototypes */
int mailbox_write(int channel, uint32_t message);
int mailbox_read(int channel, uint32_t *message);

#endif /* MAILBOX_H */
