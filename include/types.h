/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ *
 *
 * include/types.h
 *
 * C types
 *
 * Author:	Daniel Kudrow (dkudrow@cs.ucsb.edu)
 * Date:	September 25 2014
 *
 * Copyright (c) 2014, Daniel Kudrow
 * All rights reserved, see LICENSE.txt for details.
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ *
 */

#ifndef TYPES_H
#define TYPES_H

/* Boolean type */
enum { false=0, true=1 };

/* Fixed width types */
typedef char int8_t;
typedef short int16_t;
typedef int int32_t;

typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;

typedef unsigned int size_t;
typedef int off_t;

#define NULL ((void *)0)

/* Macros */
#define offsetof(type, field) \
	((size_t) &((type *)0)->field)

#define container_of(ptr, type, field) \
	((type *)((char *)(ptr)-offsetof(type, field)))

#endif /* TYPES_H */
