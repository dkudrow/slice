/*
 * src/start.S
 *
 * trampoline for main()
 */

.section .init
.globl _start

_start:
b	main		/* transfer control to OS */
