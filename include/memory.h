/*
 * include/memory.h
 *
 * BCM2835 memory layout specifics
 */

#ifndef MEMORY_H
#define MEMORY_H

 /* these aliases can be added to an address to control caching */
#define MEM_ALIAS_DIRECT	0xC0000000
#define MEM_ALIAS_L2		0x80000000
#define MEM_ALIAS_L2_CO		0x40000000
#define MEM_ALIAS_L1L2		0x00000000

#endif /* MEMORY_H */
