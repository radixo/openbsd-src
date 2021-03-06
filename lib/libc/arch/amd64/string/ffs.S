/*	$OpenBSD: ffs.S,v 1.3 2015/08/31 02:53:56 guenther Exp $	*/
/*
 * Written by J.T. Conklin <jtc@netbsd.org>.
 * Public domain.
 * Adapted for NetBSD/x86_64 by Frank van der Linden <fvdl@wasabisystems.com>
 */

#include "DEFS.h"

ENTRY(ffs)
	bsfl	%edi,%eax
	jz	L1	 		/* ZF is set if all bits are 0 */
	incl	%eax			/* bits numbered from 1, not 0 */
	ret

	_ALIGN_TEXT
L1:	xorl	%eax,%eax		/* clear result */
	ret
END_WEAK(ffs)
