/*	$OpenBSD$ */
/*
 * Written by J.T. Conklin <jtc@netbsd.org>.
 * Public domain.
 */

#include <machine/asm.h>

ENTRY(ilogbf)
	pushl	%ebp
	movl	%esp,%ebp
	subl	$4,%esp

	flds	8(%ebp)
	fxtract
	fstp	%st

	fistpl	-4(%ebp)
	movl	-4(%ebp),%eax

	leave
	ret
