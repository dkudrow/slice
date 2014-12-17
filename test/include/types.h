/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ *
 *
 * test/include/types.h
 *
 * Local C types
 *
 * Author:  Daniel Kudrow (dkudrow@cs.ucsb.edu)
 * Date:  September 25 2014
 *
 * Copyright (c) 2014, Daniel Kudrow
 * All rights reserved, see LICENSE.txt for details.
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ *
 */

#ifndef TYPES_H
#define TYPES_H

#include <stddef.h>

#define container_of(ptr, type, field) \
  ((type *)((char *)(ptr)-offsetof(type, field)))

#endif /* TYPES_H */
