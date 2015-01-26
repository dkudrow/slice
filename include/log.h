/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ *
 *
 * include/log.h
 *
 * Kernel log utility
 *
 * Author:  Daniel Kudrow (dkudrow@cs.ucsb.edu)
 * Date:  January 25 2015
 *
 * Copyright (c) 2014, Daniel Kudrow
 * All rights reserved, see LICENSE.txt for details.
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ *
 */

typedef enum { DEBUG=0, INFO=1, WARN=2, ERROR=3, QUIET=4 } log_level_t;

#define STRINGIFY(s) #s

#ifndef MODULE
#define MODULE ?
#endif

#ifdef LOGGING
#define log(level, ...)						\
{								\
	if (level >= LOGGING)					\
		_log(level, STRINGIFY(MODULE), __VA_ARGS__);	\
}
#else
#define log(...)
#endif

