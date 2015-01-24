/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ *
 *
 * src/start.S
 *
 * initialization, interrupt handlers and trampoline to OS entry
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
 * We set the stage for the kernel entry by:
 *	1. initializing each processor mode's stack pointer
 *	2. setting up the interrupt vector table
 *	3. enabling interrupts on the ARM core
 *	4. enabling interrupts on the BCM2835 peripherals 
 *
 * Before we pass control off to the OS proper, we have to take care of a
 * little housekeeping. Although we could conceivably do this in C, it is
 * much more tidy in assembly.
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ *
 */

@ ARM CPSR register fields
.set USR_MODE,	0x10		@ user mode
.set FIQ_MODE,	0x11		@ fiq mode
.set IRQ_MODE,	0x12		@ irq mode
.set SVC_MODE,	0x13		@ supervisor
.set ABT_MODE,	0x17		@ abort mode
.set UND_MODE,	0x1B		@ undefined instruction mode
.set SYS_MODE,	0x1F		@ system mode

.set IRQ_EN,	0x80		@ IRQ enable bit
.set FIQ_EN,	0x40		@ FIQ enable bit

@ Slice memory layout
.set SVC_STACK,	0x8000	@ start of system stack
.set IRQ_STACK, 0x4000		@ start of IRQ handler stack
.set FIQ_STACK, 0x3000		@ start of FIQ handler stack
.set ABT_STACK, 0x2800		@ start of ABT handler stack
.set UND_STACK, 0x2000		@ start of UND handler stack

@ Interrupt vector table
@
@ By default, the ARM's vector table resides at physical address 0x0.
@ Because the kernel image is loaded at address 0x8000, we have to be
@ careful about how the vector table is constructed. We manage this by
@ building the vector table in the .text segment and copying it into the
@ correct location manually.
@
@ Each entry in the vector table is a single (32 bit) instruction. This
@ single instruction should branch to the appropriate exception handler.
@ Because ARM instructions are fixed length, the memory we can address in
@ immediate instructions is limited. To get around this, we store the
@ addresses of the exception handlers in memory close to the vector table
@ and load them relative to the program counter. This way the handlers are
@ not bound to addresses that work with ARM's immediate addressing scheme.
@ We pay for this flexibility with an extra branch.

.section .text

.globl vectors

vectors:
ldr	pc, vec_reset		@ 0x00 -- system reset 
ldr	pc, vec_undefined	@ 0x04 -- undefined instruction
ldr	pc, vec_software_int	@ 0x08 -- software interrupt
ldr	pc, vec_prefetch_abort	@ 0x0C -- instruction fetch from bad address
ldr	pc, vec_data_abort	@ 0x10 -- data fetch from bad address
ldr	pc, vec_unused		@ 0x14 -- reserved for future use
ldr	pc, vec_irq		@ 0x18 -- interrupt request
ldr	pc, vec_fiq		@ 0x1C -- fast interrupt request

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

@ Initialization code
@
@ The ARM Core starts executing at 0x8000 so we have to make sure that
@ this is where the startup code lives. We do this by placing it in its
@ own section (.init) and instructing the linker to place this section
@ precisely at 0x8000. 

.section .init

@ Reset exception handler 
@
@ The reset exception is the highest priority exception and is signaled
@ when the ARM Core powers up. The reset handler takes care of low-level
@ initialization tasks and so we make it the entry point to our kernel
@ image. The reset handler has the following tasks:
@	1. Set up a stack for each processor mode
@	2. Set up the interrupt vector table
@	3. Enable interrupts
@	4. Branch to the kernel main

.globl handle_reset

handle_reset:		@ address of first instruction in slice!

@ Disable interrupts
mrs	r0, cpsr
orr	r0, r0, #IRQ_EN|FIQ_EN
msr	cpsr_c, r0

@ Initialize the stack pointer for each processor mode. Notice that we
@ cannot enter user mode to set the user stack pointer because there would
@ be no way of returning to supervisor mode. For this reason we enter
@ system mode which shares all of its registers with user mode but is
@ privileged (allowed to modify the CPSR register) allowing us to return
@ to supervisor mode. Also notice that we must be careful to keep
@ interrupts disabled.

ldr	sp, =SVC_STACK	@ set supervisor mode stack pointer

mrs	r0, cpsr	@ save cpsr with mode bits cleared in r0
bic	r0, r0, #SYS_MODE

orr	r1, r0, #IRQ_MODE
msr	cpsr_c, r1	@ switch to IRQ mode
ldr	sp, =IRQ_STACK	@ set IRQ mode stack pointer

orr	r1, r0, #FIQ_MODE
msr	cpsr_c, r1	@ switch to FIQ mode
ldr	sp, =FIQ_STACK	@ set FIQ mode stack pointer

orr	r1, r0, #ABT_MODE
msr	cpsr_c, r1	@ switch to ABT mode
ldr	sp, =ABT_STACK	@ set ABT mode stack pointer

orr	r1, r0, #UND_MODE
msr	cpsr_c, r1	@ switch to UND mode
ldr	sp, =UND_STACK	@ set UND mode stack pointer

orr	r1, r0, #SVC_MODE
msr	cpsr_c, r1	@ switch to SVC mode

@ Copy the vector table from .text to its home at 0x0. The following
@ instructions copy the 16 words starting at the label `vectors' into the
@ first 16 words in memory. As explained above, the first 8 words are the
@ vectors and the second 8 words are the addresses of the handlers.
ldr	r0, =vectors	@ address of vector table (in .text)
mov	r1, #0		@ address to which we will copy vector table (0x0)
ldmia	r0!, {r2-r9}	@ load all vectors
stmia	r1!, {r2-r9}	@ copy all vectors
ldmia	r0!, {r2-r9}	@ load all handlers
stmia	r1!, {r2-r9}	@ copy all handlers

@ Branch to kernel entry
b	slice_main

@ Interrupt handlers
@
@ When the ARM Core receives an interrupt request, it disables interrupts
@ and switches to corresponding mode (copying the user's CPSR into the
@ interrupt mode's CPSR). The program counter is set to the appropriate
@ location in the interrupt vector table and the interrupt handler is thus
@ invoked. Due to the structure of the ARM pipeline, the link register
@ points to one word *past* where the program was interrupted. The
@ interrupt handler must act accordingly when saving state.

.section .text

.globl handle_undefined

handle_undefined:
b	hang$

.globl handle_software_int

handle_software_int:
b	hang$

.globl handle_prefetch_abort

handle_prefetch_abort:
b	hang$

.globl handle_data_abort

handle_data_abort:
b	hang$

.globl handle_unused

handle_unused:
b	hang$

@ handle_irq
.globl handle_irq

handle_irq:
sub	lr, lr, #4		@ adjust link register
stmfd	sp!, {r0-r3, lr}	@ save context
bl	irq_handle_interrupt	@ branch to actual handler
ldmfd	sp!, {r0-r3, pc}^	@ restore context, CPSR and return

.globl handle_fiq

handle_fiq:
b	hang$

hang$:
b	hang$
