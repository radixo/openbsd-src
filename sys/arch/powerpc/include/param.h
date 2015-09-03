/*	$OpenBSD: param.h,v 1.35 2013/03/26 05:04:10 deraadt Exp $	*/

/*-
 * Copyright (C) 1995, 1996 Wolfgang Solfrank.
 * Copyright (C) 1995, 1996 TooLs GmbH.
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
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *	This product includes software developed by TooLs GmbH.
 * 4. The name of TooLs GmbH may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY TOOLS GMBH ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL TOOLS GMBH BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef	_POWERPC_PARAM_H_
#define	_POWERPC_PARAM_H_

#ifdef	_KERNEL
#ifndef	_LOCORE
#include <machine/cpu.h>
#endif	/* _LOCORE */
#endif

#define	MACHINE_ARCH	"powerpc"
#define	_MACHINE_ARCH	powerpc
#define	MID_MACHINE	MID_POWERPC

#define	PAGE_SHIFT	12
#define	PAGE_SIZE	(1 << PAGE_SHIFT)
#define	PAGE_MASK	(PAGE_SIZE - 1)

#ifdef _KERNEL

#define	NBPG		PAGE_SIZE
#define	PGSHIFT		PAGE_SHIFT
#define	PGOFSET		PAGE_MASK

#define	UPAGES		4			/* pages of u-area */
#define	USPACE		(UPAGES * PAGE_SIZE)	/* total size of u-area */
#define	USPACE_ALIGN	0			/* u-area alignment 0-none */

#define	NMBCLUSTERS	4096			/* map size, max cluster allocation */

/*
 * Maximum size of the kernel malloc arena in PAGE_SIZE-sized
 * logical pages.
 */
#define	NKMEMPAGES_MAX_DEFAULT	((64 * 1024 * 1024) >> PAGE_SHIFT)

/*
 * Fixed segments
 */
#define	PPC_USER_SR		13
#define	PPC_KERNEL_SR	14
#define	PPC_KERNEL_SEG0	0xfffff0
#define	PPC_KERNEL_SEGMENT	(PPC_KERNEL_SEG0 + PPC_KERNEL_SR)
#define	PPC_USER_ADDR	((void *)(PPC_USER_SR << ADDR_SR_SHIFT))

#endif /* _KERNEL */

#endif /* _POWERPC_PARAM_H_ */
