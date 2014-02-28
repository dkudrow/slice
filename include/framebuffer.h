/*
 * include/framebuffer.h
 *
 * framebuffer
 */

#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

/* framebuffer initialization data */
struct fb_info_t {
	unsigned width;
	unsigned height;
	unsigned virt_width;
	unsigned virt_height;
	unsigned pitch;				/* filled by GPU */
	unsigned bits_per_pixel;
	unsigned x_offset;
	unsigned y_offset;
	unsigned fb_base_addr;		/* filled by GPU */
	unsigned screen_size;		/* filled by GPU */
};

/* function prototypes */
int fb_init();
int fb_test();
int fb_border();
int fb_draw_char(unsigned x, unsigned y, unsigned c);

#endif /* FRAMEBUFFER_H */
