/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ *
 *
 * include/debug.h
 *
 * Kernel print messages
 *
 * Author:	Daniel Kudrow (dkudrow@cs.ucsb.edu)
 * Date:	March 26 2014
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ *
 */

#ifndef DEBUG_LEVEL
#define DEBUG_LEVEL 1
#endif

#ifndef PRINT_HANDLE
#define PRINT_HANDLE "?"
#endif

/*
 * Debug print levels:
 * 1: function calls
 */
#ifdef PRINT_DEBUG
#define debug_print(level, ...)\
	if (level >= DEBUG_LEVEL) {\
		printf("DEBUG (%s): ", PRINT_HANDLE);\
		printf(__VA_ARGS__);\
	}
#else
#define debug_print(...)
#endif

#ifdef PRINT_WARN
#define warn_print(...)\
	printf("WARN (%s): ", PRINT_HANDLE);\
	printf(__VA_ARGS__);
#else
#define warn_print(...)
#endif

#ifdef PRINT_ERROR
#define error_print(...)\
	printf("ERROR (%s): ", PRINT_HANDLE);\
	printf(__VA_ARGS__);
#else
#define error_print(...)
#endif
