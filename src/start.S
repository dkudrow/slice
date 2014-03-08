/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ *

 * src/start.S
 *
 * initialization and trampoline to OS entry
 *
 * Author:	Daniel Kudrow (dkudrow@cs.ucsb.edu)
 * Date:	March 7 2014
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ *
 *
 * The BCM2835's benevolent VideoCore is in charge of bringing the
 * Raspberry Pi online. There lives a bootloader in the VC's ROM and (in
 * its infinite wisdom) it expects to find a FAT32 filesystem in the first
 * partition of the SD card. The bootloader looks for `bootcode.bin',
 * `start.elf', (optionally) `config.txt' and of course `kernel.img'. The
 * kernel image is loaded into address 0x8000 (although this can be changed
 * in `config.txt') and control is reluctantly relinquished to the ARM
 * core.
 *
 * Before we pass control off to the OS proper, we have to take care of a
 * little housekeeping. Although we could conceivably do this in C, it is
 * much more tidy in assembly.
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ *
 */

.section .init		/* this should be the first thing in memory */

.globl _start		/* make sure everyone else can see this */

_start:
mov	sp, #0x8000	/* set the stack pointer */
b	slice_main	/* transfer control to OS */
