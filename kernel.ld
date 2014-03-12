/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ *
 *
 * kernel.ld
 *
 * initialization and trampoline to OS entry
 *
 * Author:	Daniel Kudrow (dkudrow@cs.ucsb.edu)
 * Date:	March 7 2014
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ *
 */

ENTRY(handle_reset)

SECTIONS
{
	.init	0x8000	: { *(.init) }
	.text		: { *(.text) }
	.data		: { *(.data) }
	.bss		: { *(.bss) }
}
