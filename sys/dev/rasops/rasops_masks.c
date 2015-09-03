/*	$OpenBSD: rasops_masks.c,v 1.4 2008/06/26 05:42:18 ray Exp $	*/
/*	$NetBSD: rasops_masks.c,v 1.5 2000/06/13 13:37:00 ad Exp $	*/

/*-
 * Copyright (c) 1999 The NetBSD Foundation, Inc.
 * All rights reserved.
 *
 * This code is derived from software contributed to The NetBSD Foundation
 * by Andrew Doran.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE NETBSD FOUNDATION, INC. AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE FOUNDATION OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include "rasops_masks.h"

/* `ragged edge' bitmasks */
#if BYTE_ORDER == BIG_ENDIAN
const
#endif
int32_t rasops_lmask[32+1] = {
	0x00000000, 0x7fffffff, 0x3fffffff, 0x1fffffff,
	0x0fffffff, 0x07ffffff, 0x03ffffff, 0x01ffffff,
	0x00ffffff, 0x007fffff, 0x003fffff, 0x001fffff,
	0x000fffff, 0x0007ffff, 0x0003ffff, 0x0001ffff,
	0x0000ffff, 0x00007fff, 0x00003fff, 0x00001fff,
	0x00000fff, 0x000007ff, 0x000003ff, 0x000001ff,
	0x000000ff, 0x0000007f, 0x0000003f, 0x0000001f,
	0x0000000f, 0x00000007, 0x00000003, 0x00000001,
	0x00000000
};

#if BYTE_ORDER == BIG_ENDIAN
const
#endif
int32_t rasops_rmask[32+1] = {
	0x00000000, 0x80000000, 0xc0000000, 0xe0000000,
	0xf0000000, 0xf8000000, 0xfc000000, 0xfe000000,
	0xff000000, 0xff800000, 0xffc00000, 0xffe00000,
	0xfff00000, 0xfff80000, 0xfffc0000, 0xfffe0000,
	0xffff0000, 0xffff8000, 0xffffc000, 0xffffe000,
	0xfffff000, 0xfffff800, 0xfffffc00, 0xfffffe00,
	0xffffff00, 0xffffff80, 0xffffffc0, 0xffffffe0,
	0xfffffff0, 0xfffffff8, 0xfffffffc, 0xfffffffe,
	0xffffffff
};

/* Part bitmasks */
#if BYTE_ORDER == BIG_ENDIAN
const
#endif
int32_t rasops_pmask[32][32] = {
      { 0xffffffff, 0x80000000, 0xc0000000, 0xe0000000,
	0xf0000000, 0xf8000000, 0xfc000000, 0xfe000000,
	0xff000000, 0xff800000, 0xffc00000, 0xffe00000,
	0xfff00000, 0xfff80000, 0xfffc0000, 0xfffe0000,
	0xffff0000, 0xffff8000, 0xffffc000, 0xffffe000,
	0xfffff000, 0xfffff800, 0xfffffc00, 0xfffffe00,
	0xffffff00, 0xffffff80, 0xffffffc0, 0xffffffe0,
	0xfffffff0, 0xfffffff8, 0xfffffffc, 0xfffffffe },

      { 0x00000000, 0x40000000, 0x60000000, 0x70000000,
	0x78000000, 0x7c000000, 0x7e000000, 0x7f000000,
	0x7f800000, 0x7fc00000, 0x7fe00000, 0x7ff00000,
	0x7ff80000, 0x7ffc0000, 0x7ffe0000, 0x7fff0000,
	0x7fff8000, 0x7fffc000, 0x7fffe000, 0x7ffff000,
	0x7ffff800, 0x7ffffc00, 0x7ffffe00, 0x7fffff00,
	0x7fffff80, 0x7fffffc0, 0x7fffffe0, 0x7ffffff0,
	0x7ffffff8, 0x7ffffffc, 0x7ffffffe, 0x7fffffff },

      { 0x00000000, 0x20000000, 0x30000000, 0x38000000,
	0x3c000000, 0x3e000000, 0x3f000000, 0x3f800000,
	0x3fc00000, 0x3fe00000, 0x3ff00000, 0x3ff80000,
	0x3ffc0000, 0x3ffe0000, 0x3fff0000, 0x3fff8000,
	0x3fffc000, 0x3fffe000, 0x3ffff000, 0x3ffff800,
	0x3ffffc00, 0x3ffffe00, 0x3fffff00, 0x3fffff80,
	0x3fffffc0, 0x3fffffe0, 0x3ffffff0, 0x3ffffff8,
	0x3ffffffc, 0x3ffffffe, 0x3fffffff, 0x00000000 },

      { 0x00000000, 0x10000000, 0x18000000, 0x1c000000,
	0x1e000000, 0x1f000000, 0x1f800000, 0x1fc00000,
	0x1fe00000, 0x1ff00000, 0x1ff80000, 0x1ffc0000,
	0x1ffe0000, 0x1fff0000, 0x1fff8000, 0x1fffc000,
	0x1fffe000, 0x1ffff000, 0x1ffff800, 0x1ffffc00,
	0x1ffffe00, 0x1fffff00, 0x1fffff80, 0x1fffffc0,
	0x1fffffe0, 0x1ffffff0, 0x1ffffff8, 0x1ffffffc,
	0x1ffffffe, 0x1fffffff, 0x00000000, 0x00000000 },

      { 0x00000000, 0x08000000, 0x0c000000, 0x0e000000,
	0x0f000000, 0x0f800000, 0x0fc00000, 0x0fe00000,
	0x0ff00000, 0x0ff80000, 0x0ffc0000, 0x0ffe0000,
	0x0fff0000, 0x0fff8000, 0x0fffc000, 0x0fffe000,
	0x0ffff000, 0x0ffff800, 0x0ffffc00, 0x0ffffe00,
	0x0fffff00, 0x0fffff80, 0x0fffffc0, 0x0fffffe0,
	0x0ffffff0, 0x0ffffff8, 0x0ffffffc, 0x0ffffffe,
	0x0fffffff, 0x00000000, 0x00000000, 0x00000000 },

      { 0x00000000, 0x04000000, 0x06000000, 0x07000000,
	0x07800000, 0x07c00000, 0x07e00000, 0x07f00000,
	0x07f80000, 0x07fc0000, 0x07fe0000, 0x07ff0000,
	0x07ff8000, 0x07ffc000, 0x07ffe000, 0x07fff000,
	0x07fff800, 0x07fffc00, 0x07fffe00, 0x07ffff00,
	0x07ffff80, 0x07ffffc0, 0x07ffffe0, 0x07fffff0,
	0x07fffff8, 0x07fffffc, 0x07fffffe, 0x07ffffff,
	0x00000000, 0x00000000, 0x00000000, 0x00000000 },

      { 0x00000000, 0x02000000, 0x03000000, 0x03800000,
	0x03c00000, 0x03e00000, 0x03f00000, 0x03f80000,
	0x03fc0000, 0x03fe0000, 0x03ff0000, 0x03ff8000,
	0x03ffc000, 0x03ffe000, 0x03fff000, 0x03fff800,
	0x03fffc00, 0x03fffe00, 0x03ffff00, 0x03ffff80,
	0x03ffffc0, 0x03ffffe0, 0x03fffff0, 0x03fffff8,
	0x03fffffc, 0x03fffffe, 0x03ffffff, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000 },

      { 0x00000000, 0x01000000, 0x01800000, 0x01c00000,
	0x01e00000, 0x01f00000, 0x01f80000, 0x01fc0000,
	0x01fe0000, 0x01ff0000, 0x01ff8000, 0x01ffc000,
	0x01ffe000, 0x01fff000, 0x01fff800, 0x01fffc00,
	0x01fffe00, 0x01ffff00, 0x01ffff80, 0x01ffffc0,
	0x01ffffe0, 0x01fffff0, 0x01fffff8, 0x01fffffc,
	0x01fffffe, 0x01ffffff, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000 },

      { 0x00000000, 0x00800000, 0x00c00000, 0x00e00000,
	0x00f00000, 0x00f80000, 0x00fc0000, 0x00fe0000,
	0x00ff0000, 0x00ff8000, 0x00ffc000, 0x00ffe000,
	0x00fff000, 0x00fff800, 0x00fffc00, 0x00fffe00,
	0x00ffff00, 0x00ffff80, 0x00ffffc0, 0x00ffffe0,
	0x00fffff0, 0x00fffff8, 0x00fffffc, 0x00fffffe,
	0x00ffffff, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000 },

      { 0x00000000, 0x00400000, 0x00600000, 0x00700000,
	0x00780000, 0x007c0000, 0x007e0000, 0x007f0000,
	0x007f8000, 0x007fc000, 0x007fe000, 0x007ff000,
	0x007ff800, 0x007ffc00, 0x007ffe00, 0x007fff00,
	0x007fff80, 0x007fffc0, 0x007fffe0, 0x007ffff0,
	0x007ffff8, 0x007ffffc, 0x007ffffe, 0x007fffff,
	0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000 },

      { 0x00000000, 0x00200000, 0x00300000, 0x00380000,
	0x003c0000, 0x003e0000, 0x003f0000, 0x003f8000,
	0x003fc000, 0x003fe000, 0x003ff000, 0x003ff800,
	0x003ffc00, 0x003ffe00, 0x003fff00, 0x003fff80,
	0x003fffc0, 0x003fffe0, 0x003ffff0, 0x003ffff8,
	0x003ffffc, 0x003ffffe, 0x003fffff, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000 },

      { 0x00000000, 0x00100000, 0x00180000, 0x001c0000,
	0x001e0000, 0x001f0000, 0x001f8000, 0x001fc000,
	0x001fe000, 0x001ff000, 0x001ff800, 0x001ffc00,
	0x001ffe00, 0x001fff00, 0x001fff80, 0x001fffc0,
	0x001fffe0, 0x001ffff0, 0x001ffff8, 0x001ffffc,
	0x001ffffe, 0x001fffff, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000 },

      { 0x00000000, 0x00080000, 0x000c0000, 0x000e0000,
	0x000f0000, 0x000f8000, 0x000fc000, 0x000fe000,
	0x000ff000, 0x000ff800, 0x000ffc00, 0x000ffe00,
	0x000fff00, 0x000fff80, 0x000fffc0, 0x000fffe0,
	0x000ffff0, 0x000ffff8, 0x000ffffc, 0x000ffffe,
	0x000fffff, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000 },

      { 0x00000000, 0x00040000, 0x00060000, 0x00070000,
	0x00078000, 0x0007c000, 0x0007e000, 0x0007f000,
	0x0007f800, 0x0007fc00, 0x0007fe00, 0x0007ff00,
	0x0007ff80, 0x0007ffc0, 0x0007ffe0, 0x0007fff0,
	0x0007fff8, 0x0007fffc, 0x0007fffe, 0x0007ffff,
	0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000 },

      { 0x00000000, 0x00020000, 0x00030000, 0x00038000,
	0x0003c000, 0x0003e000, 0x0003f000, 0x0003f800,
	0x0003fc00, 0x0003fe00, 0x0003ff00, 0x0003ff80,
	0x0003ffc0, 0x0003ffe0, 0x0003fff0, 0x0003fff8,
	0x0003fffc, 0x0003fffe, 0x0003ffff, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000 },

      { 0x00000000, 0x00010000, 0x00018000, 0x0001c000,
	0x0001e000, 0x0001f000, 0x0001f800, 0x0001fc00,
	0x0001fe00, 0x0001ff00, 0x0001ff80, 0x0001ffc0,
	0x0001ffe0, 0x0001fff0, 0x0001fff8, 0x0001fffc,
	0x0001fffe, 0x0001ffff, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000 },

      { 0x00000000, 0x00008000, 0x0000c000, 0x0000e000,
	0x0000f000, 0x0000f800, 0x0000fc00, 0x0000fe00,
	0x0000ff00, 0x0000ff80, 0x0000ffc0, 0x0000ffe0,
	0x0000fff0, 0x0000fff8, 0x0000fffc, 0x0000fffe,
	0x0000ffff, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000 },

      { 0x00000000, 0x00004000, 0x00006000, 0x00007000,
	0x00007800, 0x00007c00, 0x00007e00, 0x00007f00,
	0x00007f80, 0x00007fc0, 0x00007fe0, 0x00007ff0,
	0x00007ff8, 0x00007ffc, 0x00007ffe, 0x00007fff,
	0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000 },

      { 0x00000000, 0x00002000, 0x00003000, 0x00003800,
	0x00003c00, 0x00003e00, 0x00003f00, 0x00003f80,
	0x00003fc0, 0x00003fe0, 0x00003ff0, 0x00003ff8,
	0x00003ffc, 0x00003ffe, 0x00003fff, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000 },

      { 0x00000000, 0x00001000, 0x00001800, 0x00001c00,
	0x00001e00, 0x00001f00, 0x00001f80, 0x00001fc0,
	0x00001fe0, 0x00001ff0, 0x00001ff8, 0x00001ffc,
	0x00001ffe, 0x00001fff, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000 },

      { 0x00000000, 0x00000800, 0x00000c00, 0x00000e00,
	0x00000f00, 0x00000f80, 0x00000fc0, 0x00000fe0,
	0x00000ff0, 0x00000ff8, 0x00000ffc, 0x00000ffe,
	0x00000fff, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000 },

      { 0x00000000, 0x00000400, 0x00000600, 0x00000700,
	0x00000780, 0x000007c0, 0x000007e0, 0x000007f0,
	0x000007f8, 0x000007fc, 0x000007fe, 0x000007ff,
	0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000 },

      { 0x00000000, 0x00000200, 0x00000300, 0x00000380,
	0x000003c0, 0x000003e0, 0x000003f0, 0x000003f8,
	0x000003fc, 0x000003fe, 0x000003ff, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000 },

      { 0x00000000, 0x00000100, 0x00000180, 0x000001c0,
	0x000001e0, 0x000001f0, 0x000001f8, 0x000001fc,
	0x000001fe, 0x000001ff, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000 },

      { 0x00000000, 0x00000080, 0x000000c0, 0x000000e0,
	0x000000f0, 0x000000f8, 0x000000fc, 0x000000fe,
	0x000000ff, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000 },
     { 0x00000000, 0x00000040, 0x00000060, 0x00000070,
	0x00000078, 0x0000007c, 0x0000007e, 0x0000007f,
	0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000 },

      { 0x00000000, 0x00000020, 0x00000030, 0x00000038,
	0x0000003c, 0x0000003e, 0x0000003f, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000 },

      { 0x00000000, 0x00000010, 0x00000018, 0x0000001c,
	0x0000001e, 0x0000001f, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000 },

      { 0x00000000, 0x00000008, 0x0000000c, 0x0000000e,
	0x0000000f, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000 },

      { 0x00000000, 0x00000004, 0x00000006, 0x00000007,
	0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000 },

      { 0x00000000, 0x00000002, 0x00000003, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000 },

      { 0x00000000, 0x00000001, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000 }
};

#if BYTE_ORDER != BIG_ENDIAN
void
rasops_masks_init(void)
{
	static int flipped = 0;
	u_int i;
	u_int32_t *mask;

	if (flipped != 0)
		return;

	mask = (u_int32_t *)rasops_lmask;
	for (i = 0; i < sizeof(rasops_lmask) / sizeof(int32_t); i++)
		*mask++ = MBE(*mask);
	mask = (u_int32_t *)rasops_rmask;
	for (i = 0; i < sizeof(rasops_rmask) / sizeof(int32_t); i++)
		*mask++ = MBE(*mask);
	mask = (u_int32_t *)rasops_pmask;
	for (i = 0; i < sizeof(rasops_pmask) / sizeof(int32_t); i++)
		*mask++ = MBE(*mask);

	flipped = 1;
}
#endif
