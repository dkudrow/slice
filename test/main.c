/*
 * test/main.c
 *
 * tests
 */

#include <stdio.h>

#include "framebuffer.h"
#include "console.h"

main()
{
	char buf[64];
	while (1) {
		fgets(buf, 64, stdin);
		console_write_str(buf);
		console_flush();
	}

}
