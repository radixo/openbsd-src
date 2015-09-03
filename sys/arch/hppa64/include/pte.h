/*	$OpenBSD: pte.h,v 1.2 2010/07/24 14:30:04 kettenis Exp $	*/

/*
 * Copyright (c) 2005 Michael Shalayeff
 * All rights reserved.
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF MIND, USE, DATA OR PROFITS, WHETHER IN
 * AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT
 * OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#ifndef	_MACHINE_PTE_H_
#define	_MACHINE_PTE_H_

#define	SID_MASK	(0x1ff)
#define	SID_SHIFT	(9)
#define	PIE_MASK	(0x1ff80000000UL)
#define	PIE_SHIFT	(PDE_SHIFT+10)
#define	PDE_MASK	(0x0007fe00000UL)
#define	PDE_SHIFT	(PTE_SHIFT+9)
#define	PTE_MASK	(0x000001ff000UL)
#define	PTE_SHIFT	PAGE_SHIFT
#define	PTE_PAGE(pte)	(((pte) & PTE_PGMASK) << 7)
#define	TLB_PAGE(pg)	(((pg) >> 7) & PTE_PGMASK)

#define	PTE_IFLUSH	0x8000000000000000UL	/* software */
#define	PTE_DFLUSH	0x4000000000000000UL	/* software */
#define	PTE_REFTRAP	0x2000000000000000UL	/* used as a ref bit */
#define	PTE_DIRTY	0x1000000000000000UL
#define	PTE_BREAK	0x0800000000000000UL
#define	PTE_GATEWAY	0x04c0000000000000UL
#define	PTE_EXEC	0x0200000000000000UL
#define	PTE_WRITE	0x0100000000000000UL
#define	PTE_READ	0x0000000000000000UL
#define	PTE_USER	0x00f0000000000000UL
#define	PTE_ACC_NONE	0x0730000000000000UL
#define	PTE_ACC_MASK	0x07f0000000000000UL
#define	PTE_UNCACHABLE	0x0008000000000000UL
#define	PTE_ORDER	0x0004000000000000UL
#define	PTE_PREDICT	0x0002000000000000UL
#define	PTE_WIRED	0x0001000000000000UL	/* software */
#define	PTE_PGMASK	0x0000001fffffffe0UL
#define	PTE_PG4K	0x0000000000000000UL
#define	PTE_PG16K	0x0000000000000001UL
#define	PTE_PG64K	0x0000000000000002UL
#define	PTE_PG256K	0x0000000000000003UL
#define	PTE_PG1M	0x0000000000000004UL
#define	PTE_PG4M	0x0000000000000005UL
#define	PTE_PG16M	0x0000000000000006UL
#define	PTE_PG64M	0x0000000000000007UL

#define	PTE_PAGE_SHIFT(pte)	(12 + (2 * ((pte) & PTE_PG64M)))
#define	PTE_PAGE_SIZE(pte)	(1 << PTE_PAGE_SHIFT(pte))

#define	PTE_GETBITS(pte)	((pte) >> 48)
#define	PTE_BITS \
    "\020\01H\02P\03O\04UC\05U\010W\11X\12G\014B\015D\016REF\017FD\020FI"

#ifndef	_LOCORE
typedef	u_int64_t	pt_entry_t;
#endif

#endif	/* _MACHINE_PTE_H_ */
