/*	$OpenBSD: e_sqrt.S,v 1.3 2008/09/07 20:36:08 martynas Exp $ */
/*
 * Written by J.T. Conklin <jtc@NetBSD.org>.
 * Public domain.
 */

#include <machine/asm.h>

ENTRY(sqrt)
	sqrtsd	%xmm0,%xmm0
	ret
