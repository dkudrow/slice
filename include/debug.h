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

/*
 * Debug print levels:
 * 1: function calls
 */
#ifdef PRINT_DEBUG
#define debug_print(level, ...)\
	if (level > DEBUG_LEVEL) {\
		printf("DEBUG: ");\
		printf(__VA_ARGS__);\
	}
#else
#define debug_print(...)
#endif

#ifdef PRINT_WARN
#define warn_print(...)\
	printf("WARN: ");\
	printf(__VA_ARGS__);
#else
#define warn_print(...)
#endif

#ifdef PRINT_ERROR
#define error_print(...)\
	printf("ERROR: ");\
	printf(__VA_ARGS__);
#else
#define error_print(...)
#endif
