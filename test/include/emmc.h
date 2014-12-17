/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ *
 *
 * include/emmc.h
 *
 * External mass media controller driver
 *
 * Author:  Daniel Kudrow (dkudrow@cs.ucsb.edu)
 * Date:  March 23 2014
 *
 * Copyright (c) 2014, Daniel Kudrow
 * All rights reserved, see LICENSE.txt for details.
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ *
 */

#ifndef EMMC_H
#define EMMC_H

int emmc_init();
int emmc_read_block(unsigned block, unsigned *buf);
int emmc_write_block(unsigned block, unsigned *buf);
void emmc_dump_block(unsigned char *block);

#endif /* EMMC_H */
