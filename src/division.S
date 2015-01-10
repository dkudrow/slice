/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ *
 *
 * src/division.S
 *
 * Integer division for ARM
 *
 * Author:	Daniel Kudrow (dkudrow@cs.ucsb.edu)
 * Date:	January 15 2015
 *
 * Note: This code is adapted from 'ARM System Developer's Guide' by Sloss,
 *       Symes and Wright.
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ *
 *
 * Because the ARM does not natively support integer division, we have to
 * implement it ourselves in software. We will use the Newton-Raphson
 * method as it is one of the most efficient approximations of integer
 * division.
 *
 * The Newton-Raphson method allows us to approximate the root of a
 * function (the input that makes it zero) with the following recursion,
 *
 *	x1 = f(x0) / f'(x0)
 *
 * where x1 is a closer approximation of the root of f(x) than x0 and f'(x)
 * is the first derivative of f(x). To see how we can use this to perform
 * division, we restate the problem of *dividing* N by D as *multiplying* N
 * by the *inverse* of D. We find the inverse of D by iterating
 * Newton-Rapshon over the following equation,
 *
 *	f(x) = 1/x - D
 *
 * which of course is equal to zero when x is 1/D. Applying Newton-Raphson
 * and simplifying we get,
 *
 *	x1 = x0 * (2 - D*x0)
 *
 * eliminating the division. Our one remaining problem is that the inverse
 * of D (an integer) is not an integer so we have no way to store it. To
 * get around this we multiply by a constant that we can divide off easily
 * - a power of 2.
 * 
 * This code has a few optimizations such as the use of a lookup table to
 * generate the first estimate of the quotient. For a full treatment, see
 * the author's text.
 *
 * GCC looks for functions called _aeabi_XXX to perform software division
 * but it expects the return values in reverse order (i.e. quotient in r0
 * and remainder in r1), hence the swap at the end.
 */

.section .text

r .req r0	@ numerator and remainder
q .req r1	@ denominator and quotient
s .req r2
m .req r3
t .req r4
a .req r12

.global __aeabi_uidiv
.global __aeabi_uidivmod

__aeabi_uidiv:
__aeabi_uidivmod:
clz	s, q			@ 01 : find normalizing shift
movs	a, q, LSL s		@ 02 : perform a lookup on the
add	a, pc, a, LSR#25	@ 03 : most significant 7 bits
ldrneb	a, [a, #t32-b32-64]	@ 04 : of divisor
b32:
subs	s, s, #7		@05 : correct shift
rsb	m, q, #0		@ 06 : m = -d
movpl	q, a, LSL s		@ 07 : q approx (1 << 32)/d
@ 1st Newton iteration follows
mulpl	a, q, m			@ 08 : a = -q*d
bmi	udiv_by_large_d		@ 09 : large d trap
smlawt	q, q, a, q		@ 10 : q approx q-(q*q*d >> 32)
teq	m, m, ASR#1		@ 11 : check for d=0 or d=1
@ 2nd Newton iteration follows
mulne	a, q, m			@ 12 : a = -q*d
movne	s, #0			@ 13 : s = 0
smlalne	s, q, a, q		@ 14 : q = q-(q*q*d >> 32)
beq	udiv_by_0_or_1		@ 15 : trap d=0 or d=1
@ q now accurate enough for a remainder r, 0<=r<3*d
umull	s, q, r, q		@ 16 : q = (r*q) >> 32
add	r, r, m			@ 17 : r = n-d
mla	r, q, m, r		@ 18 : r = n-(q+1)*d
@ since 0 <= n-q*d < 3*d, thus -d <= r < 2*d
cmn	r, m			@ 19 : t = r-d
subcs	r, r, m			@ 20 : if (t<-d || t>=0) r=r+d
addcc	q, q, #1		@ 21 : if (-d<=t && t<0) q=q+1
addpl	r, r, m, LSL#1		@ 22 : if (t>=0) { r=r-2*d
addpl	q, q, #2		@ 23 : q=q+2 }
b	swap_and_ret			@ 24 : return {q, r}

udiv_by_large_d:
@ at this point we know d >= 2∧(31-6)=2∧25
sub	a, a, #4		@ 25 : set q to be an
rsb	s, s, #0		@ 26 : underestimate of
mov	q, a, LSR s		@ 27 : (1 << 32)/d
umull	s, q, r, q		@ 28 : q = (n*q) >> 32
mla	r, q, m, r		@ 29 : r = n-q*d
@ q now accurate enough for a remainder r, 0<=r<4*d
cmn	m, r, LSR#1		@ 30 : if (r/2 >= d)
addcs	r, r, m, LSL#1		@ 31 : { r=r-2*d@
addcs	q, q, #2		@ 32 : q=q+2@ }
cmn	m, r			@ 33 : if (r >= d)
addcs	r, r, m			@ 34 : { r=r-d@
addcs	q, q, #1		@ 35 : q=q+1@ }
b	swap_and_ret			@ 36 : return {q, r}

udiv_by_0_or_1:
@ carry set if d=1, carry clear if d=0
movcs	q, r			@ 37 : if (d==1) { q=n@
movcs	r, #0			@ 38 : r=0@ }
movcc	q, #-1			@ 39 : if (d==0) { q=-1@
movcc	r, #-1			@ 40 : r=-1@ }
b	swap_and_ret			@ 41 : return {q,r}

swap_and_ret:
mov	t, q
mov	q, r
mov	r, t
bx	lr

@ table for 32 by 32 bit Newton Raphson divisions
@ table[0] = 255
@ table[i] = (1 << 14)/(64+i) for i=1,2,3,...,63
t32:
.byte	0xff, 0xfc, 0xf8, 0xf4, 0xf0, 0xed, 0xea, 0xe6
.byte	0xe3, 0xe0, 0xdd, 0xda, 0xd7, 0xd4, 0xd2, 0xcf
.byte	0xcc, 0xca, 0xc7, 0xc5, 0xc3, 0xc0, 0xbe, 0xbc
.byte	0xba, 0xb8, 0xb6, 0xb4, 0xb2, 0xb0, 0xae, 0xac
.byte	0xaa, 0xa8, 0xa7, 0xa5, 0xa3, 0xa2, 0xa0, 0x9f
.byte	0x9d, 0x9c, 0x9a, 0x99, 0x97, 0x96, 0x94, 0x93
.byte	0x92, 0x90, 0x8f, 0x8e, 0x8d, 0x8c, 0x8a, 0x89
.byte	0x88, 0x87, 0x86, 0x85, 0x84, 0x83, 0x82, 0x81
