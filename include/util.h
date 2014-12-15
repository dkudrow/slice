/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ *
 *
 * include/util.h
 *
 * miscellaneous utilities that may come in handy
 *
 * Author:  Daniel Kudrow (dkudrow@cs.ucsb.edu)
 * Date:  March 7 2014
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ *
 */

#ifndef UTIL_H
#define UTIL_H


#define SWAP_ORDER_2(h) (((h & 0xFF00) >> 8) | ((h & 0x00FF) << 8))
#define SWAP_ORDER_4(w) (((w & 0xFF000000) >> 24) | ((w & 0x00FF0000) >> 8) | \
                ((w & 0x0000FF00) << 8) | ((w & 0x000000FF) << 24))

#define READ_1(addr) (*(unsigned char*)(addr))
#define READ_2(addr) (*(unsigned short*)(addr))
#define READ_4(addr) (*(unsigned int*)(addr))

#define READ_SWAP_2(addr) SWAP_ORDER_2(READ_2(addr))
#define READ_SWAP_4(addr) SWAP_ORDER_4(READ_4(addr))

/* stdio.h */
void printf(char *format_str, ...);

/* string.h */
void memcpy(void *dst, void *src, unsigned size);
void memset(void *dst, char c, unsigned size);
int strcmp(char *str1, char *str2);
char *strncpy(char *dest, const char *src, unsigned n);

#endif /* UTIL_H */
