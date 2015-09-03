/*	$OpenBSD: divrem.m4,v 1.4 2003/03/01 00:19:08 miod Exp $	*/
/*	$NetBSD: divrem.m4,v 1.7 1996/10/17 03:08:04 cgd Exp $	*/

/*
 * Copyright (c) 1994, 1995 Carnegie-Mellon University.
 * All rights reserved.
 *
 * Author: Chris G. Demetriou
 * 
 * Permission to use, copy, modify and distribute this software and
 * its documentation is hereby granted, provided that both the copyright
 * notice and this permission notice appear in all copies of the
 * software, derivative works or modified versions, and any portions
 * thereof, and that both notices appear in supporting documentation.
 * 
 * CARNEGIE MELLON ALLOWS FREE USE OF THIS SOFTWARE IN ITS "AS IS" 
 * CONDITION.  CARNEGIE MELLON DISCLAIMS ANY LIABILITY OF ANY KIND 
 * FOR ANY DAMAGES WHATSOEVER RESULTING FROM THE USE OF THIS SOFTWARE.
 * 
 * Carnegie Mellon requests users of this software to return to
 *
 *  Software Distribution Coordinator  or  Software.Distribution@CS.CMU.EDU
 *  School of Computer Science
 *  Carnegie Mellon University
 *  Pittsburgh PA 15213-3890
 *
 * any improvements or extensions that they make and grant Carnegie the
 * rights to redistribute these changes.
 */

/*
 * Division and remainder.
 *
 * The use of m4 is modeled after the sparc code, but the algorithm is
 * simple binary long division.
 *
 * Note that the loops could probably benefit from unrolling.
 */

/*
 * M4 Parameters
 * NAME		name of function to generate
 * OP		OP=div: t10 / t11 -> t12; OP=rem: t10 % t11 -> t12
 * S		S=true: signed; S=false: unsigned
 * WORDSIZE	total number of bits
 */

define(A, `t10')
define(B, `t11')
define(RESULT, `t12')

define(BIT, `t0')
define(I, `t1')
define(CC, `t2')
define(T_0, `t3')
ifelse(S, `true', `define(NEG, `t4')')

#include <machine/asm.h>

/*
 * These functions use t11 as an input, which makes them incompatible with
 * the secureplt calling sequence. The compiler knows about this, and will
 * ask for a call through a got relocation. But this can only work if the
 * linker omits creating a plt entry for the symbol. In order to achieve
 * this, we need to declare it as `notype' instead of `function', which
 * means that LEAF(NAME, 0) can't be used as it uses .ent which forces the
 * `function' type.
 */
	.globl	NAME
	.type	NAME, @notype
	.usepv	NAME, no

	.cfi_startproc
	.cfi_return_column ra
NAME:
	MCOUNT
	lda	sp, -64(sp)
	.cfi_def_cfa_offset 64
	stq	BIT, 0(sp)
	.cfi_rel_offset BIT, 0
	stq	I, 8(sp)
	.cfi_rel_offset I, 8
	stq	CC, 16(sp)
	.cfi_rel_offset CC, 16
	stq	T_0, 24(sp)
	.cfi_rel_offset T_0, 24
ifelse(S, `true',`dnl
	stq	NEG, 32(sp)
	.cfi_rel_offset NEG, 32
')dnl
	stq	A, 40(sp)
	.cfi_rel_offset A, 40
	stq	B, 48(sp)
	.cfi_rel_offset B, 48
	mov	zero, RESULT			/* Initialize result to zero */

ifelse(S, `true',
`
	/* Compute sign of result.  If either is negative, this is easy.  */
	or	A, B, NEG			/* not the sign, but... */
	srl	NEG, WORDSIZE - 1, NEG		/* rather, or of high bits */
	blbc	NEG, Ldoit			/* neither negative? do it! */

ifelse(OP, `div',
`	xor	A, B, NEG			/* THIS is the sign! */
', `	mov	A, NEG				/* sign follows A. */
')
	srl	NEG, WORDSIZE - 1, NEG		/* make negation the low bit. */

	srl	A, WORDSIZE - 1, I		/* is A negative? */
	blbc	I, LnegB			/* no. */
	/* A is negative; flip it. */
ifelse(WORDSIZE, `32', `
	/* top 32 bits may be random junk */
	zap	A, 0xf0, A
')
	subq	zero, A, A
	srl	B, WORDSIZE - 1, I		/* is B negative? */
	blbc	I, Ldoit			/* no. */
LnegB:
	/* B is definitely negative, no matter how we got here. */
ifelse(WORDSIZE, `32', `
	/* top 32 bits may be random junk */
	zap	B, 0xf0, B
')
	subq	zero, B, B
Ldoit:
')
ifelse(WORDSIZE, `32', `
	/*
	 * Clear the top 32 bits of each operand, as they may
	 * sign extension (if negated above), or random junk.
	 */
	zap	A, 0xf0, A
	zap	B, 0xf0, B
')

	/* kill the special cases. */
	beq	B, Ldotrap			/* division by zero! */

	cmpult	A, B, CC			/* A < B? */
	/* RESULT is already zero, from above.  A is untouched. */
	bne	CC, Lret_result

	cmpeq	A, B, CC			/* A == B? */
	cmovne	CC, 1, RESULT
	cmovne	CC, zero, A
	bne	CC, Lret_result

	/*
	 * Find out how many bits of zeros are at the beginning of the divisor.
	 */
LBbits:
	ldiq	T_0, 1				/* I = 0; BIT = 1<<WORDSIZE-1 */
	mov	zero, I
	sll	T_0, WORDSIZE-1, BIT
LBloop:
	and	B, BIT, CC			/* if bit in B is set, done. */
	bne	CC, LAbits
	addq	I, 1, I				/* increment I, shift bit */
	srl	BIT, 1, BIT
	cmplt	I, WORDSIZE-1, CC		/* if I leaves one bit, done. */
	bne	CC, LBloop

LAbits:
	beq	I, Ldodiv			/* If I = 0, divide now.  */
	ldiq	T_0, 1				/* BIT = 1<<WORDSIZE-1 */
	sll	T_0, WORDSIZE-1, BIT

LAloop:
	and	A, BIT, CC			/* if bit in A is set, done. */
	bne	CC, Ldodiv
	subq	I, 1, I				/* decrement I, shift bit */
	srl     BIT, 1, BIT 
	bne	I, LAloop			/* If I != 0, loop again */

Ldodiv:
	sll	B, I, B				/* B <<= i */
	ldiq	T_0, 1
	sll	T_0, I, BIT

Ldivloop:
	cmpult	A, B, CC
	or	RESULT, BIT, T_0
	cmoveq	CC, T_0, RESULT
	subq	A, B, T_0
	cmoveq	CC, T_0, A
	srl	BIT, 1, BIT	
	srl	B, 1, B
	beq	A, Lret_result
	bne	BIT, Ldivloop

Lret_result:
ifelse(OP, `div',
`', `	mov	A, RESULT
')
ifelse(S, `true',
`
	/* Check to see if we should negate it. */
	subq	zero, RESULT, T_0
	cmovlbs	NEG, T_0, RESULT
')

	ldq	BIT, 0(sp)
	.cfi_restore BIT
	ldq	I, 8(sp)
	.cfi_restore I
	ldq	CC, 16(sp)
	.cfi_restore CC
	ldq	T_0, 24(sp)
	.cfi_restore T_0
ifelse(S, `true',`dnl
	ldq	NEG, 32(sp)
	.cfi_restore NEG
')dnl
	ldq	A, 40(sp)
	.cfi_restore A
	ldq	B, 48(sp)
	.cfi_restore B
	lda	sp, 64(sp)
	.cfi_def_cfa_offset 0
	ret	zero, (t9), 1

Ldotrap:
	ldiq	a0, -2			/* This is the signal to SIGFPE! */
	call_pal PAL_gentrap
ifelse(OP, `div',
`', `	mov	zero, A			/* so that zero will be returned */
')
	br	zero, Lret_result

/*
 * For the reasons stated above, we can not use END(NAME) either, as it
 * expands to .end which requires a matching .ent.
 */
	.cfi_endproc
	.size	NAME, . - NAME
