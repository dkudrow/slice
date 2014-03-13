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

#define CPSR_MODE_USR		0x10	/* user mode mask */
#define CPSR_MODE_FIQ		0x11	/* fiq mode mask */
#define CPSR_MODE_IRQ		0x12	/* irq mode mask */
#define CPSR_MODE_SVC		0x13	/* supervisor mode mask */
#define CPSR_MODE_ABT		0x17	/* abort mode mask */
#define CPSR_MODE_UND		0x1B	/* undefined instruction mode mask */
#define CPSR_MODE_SYS		0x1F	/* system mode mask */

#define CPSR_ENABLE_IRQ		0x80	/* IRQ enable mask */
#define CPSR_ENABLE_FIQ		0x40	/* FIQ enable mask */
#define CPSR_ENABLE_INT		0xC0	/* all interrupt enable mask */

/*
 * vector table
 *
 * By default, the ARM's vector table resides at physical address 0x0.
 * Because the kernel image is loaded at address 0x8000, we have to be
 * careful about how the vector table is constructed. We manage this by
 * building the vector table in the .text segment and copying it into the
 * correct location manually.
 *
 * Each entry in the vector table is a single (32 bit) instruction. This
 * single instruction should branch to the appropriate exception handler.
 * Because ARM instructions are fixed length, the memory we can address in
 * immediate instructions is limited. To get around this, we store the
 * addresses of the exception handlers in memory close to the vector table
 * and load them relative to the program counter. This way the handlers are
 * not bound to addresses that work with ARM's immediate addressing scheme.
 * We pay for this flexibility by introducing another layer of
 * indirection.
 */
.globl vectors

vectors:
ldr	pc, vec_reset		/* 0x00 -- system reset */
ldr	pc, vec_undefined	/* 0x04 -- undefined instruction */
ldr	pc, vec_software_int	/* 0x08 -- software interrupt */
ldr	pc, vec_prefetch_abort	/* 0x0C -- instruction fetch from bad address */
ldr	pc, vec_data_abort	/* 0x10 -- data fetch from bad address */
ldr	pc, vec_unused		/* 0x14 -- reserved for future use */
ldr	pc, vec_irq		/* 0x18 -- interrupt request */
ldr	pc, vec_fiq		/* 0x1C -- fast interrupt request */

vec_reset:
.word handle_reset

vec_undefined:
.word handle_undefined

vec_software_int:
.word handle_software_int

vec_prefetch_abort:
.word handle_prefetch_abort

vec_data_abort:
.word handle_data_abort

vec_unused:
.word handle_unused

vec_irq:
.word handle_irq

vec_fiq:
.word handle_fiq

/*
 * initialization code
 *
 * The ARM Core starts executing at 0x8000 so we have to make sure that
 * this is where the startup code lives. We do this by placing it in its
 * own section (.init) and instructing the linker to place this section
 * precisely at 0x8000. 
 */
.section .init

/*
 * reset exception handler 
 *
 * The reset exception is the highest priority exception and is signaled
 * when the ARM Core powers up. The reset handler takes care of low-level
 * initialization tasks and so we make it the entry point to our kernel
 * image. The reset handler has the following tasks:
 *	1. Set up a stack for each processor mode
 *	2. Set up the vector table and enable exceptions
 */
.globl handle_reset

handle_reset:		/* address of first instruction in slice! */

/*
 * Initialize the stack pointer for each processor mode. Notice that we
 * cannot enter user mode to set the user stack pointer because there would
 * be no way of returning to supervisor mode. For this reason we enter
 * system mode which shares all of its registers with user mode but
 * privileged (allowed to modify the cpsr register) allowing us to return
 * to supervisor mode.
 */
ldr	sp, =0x8000	/* set supervisor mode stack pointer */

mov	r0, #CPSR_MODE_IRQ|CPSR_ENABLE_INT
msr	cpsr, r0	/* change to IRQ mode */
ldr	sp, =0x7000	/* set IRQ mode stack pointer */

mov	r0, #CPSR_MODE_FIQ|CPSR_ENABLE_INT
msr	cpsr, r0	/* change to FIQ mode */
ldr	sp, =0x6000	/* set FIQ mode stack pointer */

mov	r0, #CPSR_MODE_SYS|CPSR_ENABLE_INT
msr	cpsr, r0	/* change to system mode */
ldr	sp, =0x5000	/* set user/system mode stack pointer */

mov	r0, #CPSR_MODE_UND|CPSR_ENABLE_INT
msr	cpsr, r0	/* change to undefined instruction mode */
ldr	sp, =0x4000	/* set undefined instruction mode stack pointer */

mov	r0, #CPSR_MODE_ABT|CPSR_ENABLE_INT
msr	cpsr, r0	/* change to abort mode */
ldr	sp, =0x3000	/* set abort mode stack pointer */

/*
 * Copy the vector table from .text to its home at 0x0.
 */
ldr	r0, =vectors	/* address of vector table (in .text) */
mov	r1, #0		/* address to which we will copy vector table (0x0) */
ldmia	r0!, {r2-r9}	/* load all vectors */
stmia	r1!, {r2-r9}	/* copy all vectors */
ldmia	r0!, {r2-r9}	/* load all handlers */
stmia	r1!, {r2-r9}	/* copy all handlers */

b	slice_main	/* transfer control to OS */

/*
 * FIXME:
 * These handlers are located here temporarily but will eventually find
 * their way into .text
 */
handle_undefined:
b	hang$

handle_software_int:
b	hang$

handle_prefetch_abort:
b	hang$

handle_data_abort:
b	hang$

handle_unused:
b	hang$

handle_irq:
b	hang$

handle_fiq:
b	hang$

hang$:
b	hang$

