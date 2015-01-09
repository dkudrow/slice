/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ *
 *
 * src/framebuffer.c
 *
 * BCM2835 framebuffer
 *
 * Author:	Daniel Kudrow (dkudrow@cs.ucsb.edu)
 * Date:	March 7 2014
 *
 * Copyright (c) 2014, Daniel Kudrow
 * All rights reserved, see LICENSE.txt for details.
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ *
 *
 * In order to write pixels to a display, we have to request a framebuffer
 * from the VideoCore. We do this by allocating a special framebuffer info
 * structure (defined by the VideoCore specification) and passing the
 * address to the framebuffer's mailbox channel. The structure must be
 * 16-byte aligned because mailbox data can only use 28 bits - the low
 * 4 bits of the address will are clobbered by the channel field. If all
 * goes according to plan, the VC fills in the address of the framebuffer
 * in our structure and sends us a 0 in the mailbox. To draw a pixel on the
 * screen we simply write the corresponding pixels in the framebuffer.
 *
 * Layout of the framebuffer info structure:
 *
 * 	offset		function
 * 	---------------------
 * 	0x00		Screen width
 * 	0x04		Screen height
 * 	0x08		Virtual width (???)
 * 	0x0C		Virtual height (???)
 * 	0x10		*Pitch -- the byte-length of one 'row' of the framebuffer
 * 	0x14		Depth -- number of bits per pixel
 * 	0x18		X offset (???)
 * 	0x1C		Y offset (???)
 * 	0x20		*Base address of framebuffer
 * 	0x24		*Screen size
 *
 * 	*filled in by VideoCore
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ *
 */

#include <framebuffer.h>
#include <mailbox.h>
#include <memory.h>
#include <string.h>
#include <util.h>

#include "font.h"

struct fb_info_t fb_info __attribute__ ((aligned (16)));

/*
 * query the VideoCore for the framebuffer
 */
int fb_init()
{
	unsigned fb_addr;

	fb_info.width = 1024;
	fb_info.height = 768;
	fb_info.virt_width = 1024;
	fb_info.virt_height = 768;
	fb_info.pitch = 0;
	fb_info.bits_per_pixel = 16;
	fb_info.x_offset = 0;
	fb_info.y_offset = 0;
	fb_info.fb_base_addr = 0;
	fb_info.screen_size = 0;

	/* send the framebuffer info to the GPU, don't use L1 cache */
	mailbox_write(MBOX_CHAN_FB, (unsigned)&fb_info + MEM_ALIAS_L2_CO);

	/* recieve response from GPU */
	mailbox_read(MBOX_CHAN_FB, &fb_addr);

	/* if the response is not zero, there has been an error */
	if (fb_addr != 0)
		return -1;

	return 0;
}

/*
 * test framebuffer
 */
int fb_test()
{
	unsigned short color;
	unsigned fb;
	int x, y;

	color = 0x0;

	while (1) {
		fb = (unsigned) fb_info.fb_base_addr;
		for (y=0; y<fb_info.height; y++) {
			for (x=0; x<fb_info.width; x++) {
				fb += 2;
				*(unsigned short *)fb = color;
			}
			++color;
		}
	}
}

/*
 * draw a border around the framebuffer
 */
int fb_border()
{
	int i;
	unsigned short color = 0xFFFF;
	unsigned addr = fb_info.fb_base_addr;

	for (i=0; i<fb_info.width; i++) {
		*((unsigned short *)addr + i) = color;
		*((unsigned short *)addr + fb_info.width*(fb_info.height-1) + i) = color;
	}

	for (i=0; i<fb_info.height; i++) {
		*((unsigned short *)addr + i*fb_info.width) = color;
		*((unsigned short *)addr + i*fb_info.width + fb_info.width-1) = color;
	}
}

/*
 * scroll y lines
 */
int fb_scroll(int rows, int y)
{
	unsigned pitch = fb_info.pitch;
	unsigned base = fb_info.fb_base_addr;
	unsigned start_addr = base + y*FONT_HEIGHT*pitch;
	unsigned end_addr = base + (rows-y)*FONT_HEIGHT*pitch;

	/* shift the bottom rows-y lines to the top of the screen */
	memcpy((void *)base, (void *)start_addr, (rows-y)*FONT_HEIGHT*pitch);

	/* clear the bottom y lines */
	memset((void *)end_addr, 0, y*FONT_HEIGHT*pitch);
}

/*
 * draw a single bitmapped character
 */
int fb_draw_char(unsigned x, unsigned y, unsigned c)
{
	int  i, j;
	unsigned short color = 0xFFFF;
	unsigned addr = fb_info.fb_base_addr;

	for (j=0; j<FONT_HEIGHT; j++) {
		for (i=0; i<FONT_WIDTH; i++) {
			if (bitmap[FONT_HEIGHT*c + j] & (128>>i)) {
				*((unsigned short *)addr + (y*FONT_HEIGHT+j)*fb_info.width + (x*FONT_WIDTH+i)) = color;
			}
		}
	}

	return 0;
}
