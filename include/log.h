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

#ifndef LOGGING
#define LOGGING QUIET
#endif

#ifndef MODULE
#define MODULE ???
#endif

#define ADD_QUOTE(macro) #macro
#define TO_STRING(macro) ADD_QUOTE(macro)

#define MODULE_NAME TO_STRING(MODULE)

#define log(level, ...)						\
{								\
	if (level >= LOGGING)					\
		_log(level, MODULE_NAME, __VA_ARGS__);		\
}
