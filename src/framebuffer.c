/*
 * src/framebuffer.c
 *
 * framebuffer
 */

#include "framebuffer.h"
#include "mailbox.h"
#include "memory.h"

#include "font.h"

/*
 * The pointer we pass to the mailbox must be aligned to 16 bytes because
 * the mailbox uses the low 4 bits of the read register to hold the
 * channel. Since the address is a 32 bit value, we must ensure that the
 * low 4 bits carry no information.
 */
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
		*((unsigned short *)addr + i) = color;;
		*((unsigned short *)addr + fb_info.width*(fb_info.height-1) + i) = color;;
	}

	for (i=0; i<fb_info.height; i++) {
		*((unsigned short *)addr + i*fb_info.width) = color;;
		*((unsigned short *)addr + i*fb_info.width + fb_info.width-1) = color;;
	}
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
