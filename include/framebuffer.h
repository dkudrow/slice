/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ *
 *
 * include/framebuffer.h
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
 */

#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

/* Framebuffer parameters */
#define FB_WIDTH 1024
#define FB_HEIGHT 768
#define FG_COLOR 0xFFFF
#define BG_COLOR 0x0000

typedef unsigned short pixel_t;

/* Function prototypes */
int fb_init();
int fb_test();
int fb_border();
int fb_scroll(int rows, int y);
int fb_draw_char(int x, int y, char c);

#endif /* FRAMEBUFFER_H */
