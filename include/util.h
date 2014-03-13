/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ *
 *
 * include/util.h
 *
 * miscellaneous utilities that may come in handy
 *
 * Author:	Daniel Kudrow (dkudrow@cs.ucsb.edu)
 * Date:	March 7 2014
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ *
 */

#ifndef UTIL_H
#define UTIL_H

/* stdio.h */
void printf(char *format_str, ...);

/* string.h */
void memcpy(void *dst, void *src, unsigned size);
void memset(void *dst, char c, unsigned size);

#endif /* UTIL_H */
