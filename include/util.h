/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ *
 *
 * include/util.h
 *
 * Miscellaneous kernel utilities
 *
 * Author:	Daniel Kudrow (dkudrow@cs.ucsb.edu)
 * Date:	March 7 2014
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ *
 */

#ifndef UTIL_H
#define UTIL_H

#include <types.h>

#define SWAP_ORDER_2(h) (((h & 0xFF00) >> 8) | ((h & 0x00FF) << 8))
#define SWAP_ORDER_4(w) (((w & 0xFF000000) >> 24) | ((w & 0x00FF0000) >> 8) | \
		((w & 0x0000FF00) << 8) | ((w & 0x000000FF) << 24))

#define READ1(addr) (*(uint8_t *)(addr))
#define READ2(addr) (*(uint16_t *)(addr))
#define READ4(addr) (*(uint32_t *)(addr))

#define READ_SWAP_2(addr) SWAP_ORDER_2(READ_2(addr))
#define READ_SWAP_4(addr) SWAP_ORDER_4(READ_4(addr))

#define WRITE1(addr, val) *(uint8_t *)(addr) = (val)
#define WRITE2(addr, val) *(uint16_t *)(addr) = (val)
#define WRITE4(addr, val) *(uint32_t *)(addr) = (val)

#define MIN(a, b) ((a) > (b) ? (b) : (a))
#define MAX(a, b) ((a) < (b) ? (b) : (a))

void kprintf(char *format_str, ...);

#endif /* UTIL_H */
