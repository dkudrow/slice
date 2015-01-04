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

/* misc */
#define MIN(a, b) ((a) > (b) ? (b) : (a))
#define MAX(a, b) ((a) < (b) ? (b) : (a))

/* stdio.h */
void printf(char *format_str, ...);

/* string.h */
void *memcpy(void *dst, const void *src, unsigned long size);
void *memset(void *dst, int c, unsigned long size);
int strcmp(const char *str1, const char *str2);
int strncmp(const char *str1, const char *str2, size_t n);
char *strncpy(char *dst, const char *src, unsigned long n);
size_t strlen(const char *str);
int toupper(int c) /* FIXME: should live in ctype.h */

#endif /* UTIL_H */
