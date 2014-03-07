/*
 * src/start.S
 *
 * trampoline for main()
 */

.section .init
.globl _start

_start:
mov	sp, #0x8000	/* set the stack pointer */
b	slice_main	/* transfer control to OS */
