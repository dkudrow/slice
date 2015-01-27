/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ *
 *
 * src/log.c
 *
 * Kernel logging utility
 *
 * Author:	Daniel Kudrow (dkudrow@cs.ucsb.edu)
 * Date:	January 25 2015
 *
 * Copyright (c) 2014, Daniel Kudrow
 * All rights reserved, see LICENSE.txt for details.
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ *
 */
 
#include <util.h>
#include <log.h>

#define log_print(...) kprintf(__VA_ARGS__)
#define log_vprint(f, a) kvprintf(f, a)

void _log(log_level_t level, const char *module, const char *format_str, ...)
{
	va_list arg_list;
	va_start(arg_list, format_str);

	switch(level) {
		case DEBUG:
			log_print("[DEBUG] ");
			break;
		case WARN:
			log_print("[WARN]  ");
			break;
		case INFO:
			log_print("[INFO]  ");
			break;
		case ERROR:
			log_print("[ERROR] ");
			break;
		default:
			log_print("[?] ");
	}
	log_print("%s: ", module);
	log_vprint(format_str, arg_list);
	log_print("\n");

	va_end(arg_list);
}



