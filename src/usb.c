/*
 * src/usb.c
 *
 * first crack at a usb driver
 */

/*
 * tell the VideoCore to power up the USB host controller
 */

#include "mailbox.h"

usb_power_on()
{
	unsigned ret;

	mailbox_write(MBOX_CHAN_PWR, 0x80);
	mailbox_read(MBOX_CHAN_PWR, &ret);

	return (ret == 0x80) ? 0 : -1;
}

