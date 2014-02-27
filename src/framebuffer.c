/*
 * src/framebuffer.c
 *
 * framebuffer
 */

#include "framebuffer.h"
#include "mailbox.h"
#include "memory.h"

#include "font.h"

/* aligned to 16 bytes b/c mailbox channel uses low 4 bits */
struct fb_info_t fb_info __attribute__ ((aligned (16))) = {
	.width = 1024,
	.height = 768,
	.virt_width = 1024,
	.virt_height = 768,
	.pitch = 0,
	.bits_per_pixel = 16,
	.x_offset = 0,
	.y_offset = 0,
	.fb_base_addr = 0,
	.screen_size = 0
};

/*
 * initialize the frame buffer
 */
int fb_init()
{
	unsigned fb_addr;

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
 * draw a single bitmapped character
 */
int console_draw_char(unsigned x, unsigned y, unsigned c)
{
	int  i, j;
	unsigned short color = 0xFFFF;
	unsigned addr = fb_info.fb_base_addr;

	for (j=0; j<16; j++) {
		for (i=0; i<8; i++) {
			if (bitmap[16*c + j] & (1<<i)) {
				*((unsigned short *)addr + (y*16+j)*fb_info.width + (x*8+i)) = color;
			}
		}
	}

	return 0;
}
