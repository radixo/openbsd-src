/*	$OpenBSD: float.h,v 1.9 2011/08/29 13:13:21 kettenis Exp $	*/

/*
 * Copyright (c) 1989 Regents of the University of California.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#ifndef _MACHINE__FLOAT_H_
#define _MACHINE__FLOAT_H_

#define __FLT_RADIX		2		/* b */
#define __FLT_ROUNDS		1		/* FP addition rounds to nearest */
#define __FLT_EVAL_METHOD	0		/* no promotions */

#define __FLT_MANT_DIG		24		/* p */
#define __FLT_EPSILON		1.19209290E-7F	/* b**(1-p) */
#define __FLT_DIG		6		/* floor((p-1)*log10(b))+(b == 10) */
#define __FLT_MIN_EXP		(-127)		/* emin */
#define __FLT_MIN		2.93873588E-39F	/* b**(emin-1) */
#define __FLT_MIN_10_EXP	(-38)		/* ceil(log10(b**(emin-1))) */
#define __FLT_MAX_EXP		127		/* emax */
#define __FLT_MAX		1.70141173E+38F	/* (1-b**(-p))*b**emax */
#define __FLT_MAX_10_EXP	38		/* floor(log10((1-b**(-p))*b**emax)) */

#define __DBL_MANT_DIG		56
#define __DBL_EPSILON		2.77555756156289135E-17
#define __DBL_DIG		16
#define __DBL_MIN_EXP		(-127)
#define __DBL_MIN		2.938735877055718770E-39
#define __DBL_MIN_10_EXP	(-38)
#define __DBL_MAX_EXP		127
#define __DBL_MAX		1.701411834604692294E+38
#define __DBL_MAX_10_EXP	38

#define __LDBL_MANT_DIG		DBL_MANT_DIG
#define __LDBL_EPSILON		DBL_EPSILON
#define __LDBL_DIG		DBL_DIG
#define __LDBL_MIN_EXP		DBL_MIN_EXP
#define __LDBL_MIN		DBL_MIN
#define __LDBL_MIN_10_EXP	DBL_MIN_10_EXP
#define __LDBL_MAX_EXP		DBL_MAX_EXP
#define __LDBL_MAX		DBL_MAX
#define __LDBL_MAX_10_EXP	DBL_MAX_10_EXP

#define __DECIMAL_DIG		18

#endif /* _MACHINE__FLOAT_H_ */
