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
 *	offset		function
 *	---------------------
 *	0x00		Screen width
 *	0x04		Screen height
 *	0x08		Virtual width (???)
 *	0x0C		Virtual height (???)
 *	0x10		*Pitch -- the byte-length of one 'row' of the framebuffer
 *	0x14		Depth -- number of bits per pixel
 *	0x18		X offset (???)
 *	0x1C		Y offset (???)
 *	0x20		*Base address of framebuffer
 *	0x24		*Screen size
 *
 *	*filled in by VideoCore
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ *
 */

#include <framebuffer.h>
#include <mailbox.h>
#include <memory.h>
#include <string.h>
#include <util.h>

#include "font.h"

/*
 * Framebuffer initialization data
 */
volatile struct __attribute__ ((aligned (16))) {
	uint32_t width;
	uint32_t height;
	uint32_t virt_width;
	uint32_t virt_height;
	uint32_t pitch;			/* filled by GPU */
	uint32_t bits_per_pixel;
	uint32_t x_offset;
	uint32_t y_offset;
	uint32_t fb_base_addr;		/* filled by GPU */
	uint32_t screen_size;		/* filled by GPU */
} fb_info;

/*
 * Query the VideoCore for the framebuffer
 */
int fb_init()
{
	int status;

	fb_info.width = FB_WIDTH;
	fb_info.height = FB_HEIGHT;
	fb_info.virt_width = FB_WIDTH;
	fb_info.virt_height = FB_HEIGHT;
	fb_info.pitch = 0;
	fb_info.bits_per_pixel = 8*sizeof(pixel_t);
	fb_info.x_offset = 0;
	fb_info.y_offset = 0;
	fb_info.fb_base_addr = 0;
	fb_info.screen_size = 0;

	/* send the framebuffer info to the GPU, don't use L1 cache */
	mailbox_write(MBOX_CHAN_FB, (uint32_t)&fb_info + MEM_ALIAS_L2_CO);

	/* recieve response from GPU */
	mailbox_read(MBOX_CHAN_FB, &status);

	/* if the response is not zero, there has been an error */
	if (status != 0)
		return -1;

	return 0;
}

/*
 * Test framebuffer
 */
int fb_test()
{
	pixel_t color;
	pixel_t *fb;
	int x, y;

	color = 0x0;

	while (1) {
		fb = (pixel_t *)fb_info.fb_base_addr;
		for (y=0; y<fb_info.height; y++) {
			for (x=0; x<fb_info.width; x++) {
				++fb;
				WRITE2(fb, color);
			}
			++color;
		}
	}
}

/*
 * Draw a border around the framebuffer
 */
int fb_border()
{
	int i;
	pixel_t color = 0xFFFF;
	pixel_t *addr = (pixel_t *)fb_info.fb_base_addr;

	for (i=0; i<fb_info.width; i++) {
		/**((unsigned short *)addr + i) = color;*/
		/**((unsigned short *)addr + fb_info.width*(fb_info.height-1) + i) = color;*/
		WRITE2(addr+i, color);
		WRITE2(addr + fb_info.width*(fb_info.height-1) + i, color);
	}

	for (i=0; i<fb_info.height; i++) {
		/**((unsigned short *)addr + i*fb_info.width) = color;*/
		/**((unsigned short *)addr + i*fb_info.width + fb_info.width-1) = color;*/
		WRITE2(addr + i*fb_info.width, color);
		WRITE2(addr + i*fb_info.width + fb_info.width-1, color);
	}
}

/*
 * Scroll y lines
 */
int fb_scroll(int rows, int y)
{
	unsigned pitch = fb_info.pitch;
	pixel_t *base = (pixel_t *)fb_info.fb_base_addr;
	pixel_t *start_addr = base + y*FONT_HEIGHT*pitch;
	pixel_t *end_addr = base + (rows-y)*FONT_HEIGHT*pitch;

	/* shift the bottom rows-y lines to the top of the screen */
	memcpy(base, start_addr, (rows-y)*FONT_HEIGHT*pitch);

	/* clear the bottom y lines */
	memset(end_addr, BG_COLOR, y*FONT_HEIGHT*pitch);
}

/*
 * Draw a single bitmapped character
 */
int fb_draw_char(int x, int y, char c)
{
	int i, j;
	pixel_t *addr = (pixel_t *)fb_info.fb_base_addr;

	for (j=0; j<FONT_HEIGHT; j++) {
		for (i=0; i<FONT_WIDTH; i++) {
			if (bitmap[FONT_HEIGHT*c + j] & (128>>i)) {
				WRITE2(addr + (y*FONT_HEIGHT+j)*fb_info.width +
						(x*FONT_WIDTH+i), FG_COLOR);
			}
		}
	}

	return 0;
}
