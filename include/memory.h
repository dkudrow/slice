/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ *
 *
 * include/memory.h
 *
 * BCM2835 memory layout specifics
 *
 * Author:	Daniel Kudrow (dkudrow@cs.ucsb.edu)
 * Date:	March 7 2014
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ *
 */

#ifndef MEMORY_H
#define MEMORY_H

 /* these aliases can be added to an address to control caching */
#define MEM_ALIAS_DIRECT	0xC0000000
#define MEM_ALIAS_L2		0x80000000
#define MEM_ALIAS_L2_CO		0x40000000
#define MEM_ALIAS_L1L2		0x00000000

#define readb(addr) (*(unsigned char*)addr)
#define writeb(addr, byte) (*(unsigned char*)addr) = (unsigned char)byte

#define reads(addr) (*(unsigned short*)addr)
#define writes(addr, word) (*(unsigned short*)addr) = (unsigned short)word

#define readw(addr) (*(unsigned *)addr)
#define writew(addr, word) (*(unsigned *)addr) = (unsigned)word

#endif /* MEMORY_H */
