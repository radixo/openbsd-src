/* $OpenBSD: md_init.h,v 1.6 2014/12/23 12:25:43 kettenis Exp $ */

/*
 * Copyright (c) 2003 Dale Rahn. All rights reserved.
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
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */  


#ifdef __PIC__
#define MD_SECT_CALL_FUNC(section, func)			\
	__asm (".section "#section",\"ax\",@progbits	\n"	\
	"	bl	" #func ",%r2			\n"	\
	"	stw	%r19,-80(%r30)			\n"	\
	"	ldw	-80(%r30),%r19			\n"	\
	"	.previous")
#else
#define MD_SECT_CALL_FUNC(section, func)			\
	__asm (".section .rodata			\n"	\
	"	.align 4				\n"	\
	"L$" #func "					\n"	\
	"	.word "#func "				\n"	\
	"	.previous				\n"	\
	"	.section "#section",\"ax\",@progbits	\n"	\
	"	ldil	LR'L$" #func ",%r1		\n"	\
	"	ldw	RR'L$" #func "(%r1), %r31	\n"	\
	"	ble	0(%sr4,%r31)			\n"	\
	"	copy 	%r31,%r2			\n"	\
	"	.previous")
#endif

#define MD_SECTION_PROLOGUE(sect, entry_pt)			\
	__asm (						   	\
	"	.section "#sect",\"ax\",@progbits	\n"	\
	"	.EXPORT "#entry_pt",ENTRY,PRIV_LEV=3,ARGW0=NO,ARGW1=NO,ARGW2=NO,ARGW3=NO,RTNVAL=NO					\n"	\
	"	.align 4				\n"	\
	#entry_pt"					\n"	\
	"	stw %r2, -20(%r30)			\n"	\
	"	ldo 64(%r30),%r30			\n"	\
	"	/* fall thru */				\n"	\
	"	.previous")


#define MD_SECTION_EPILOGUE(sect)				\
	__asm (							\
	"	.section "#sect",\"ax\",@progbits	\n"	\
	"	ldw -84(%r30),%r2			\n"	\
	"	bv %r0(%r2)				\n"	\
	"	ldo -64(%r30),%r30			\n"	\
	"	.previous")


#include <sys/exec.h>		/* for struct psstrings */

#define	MD_CRT0_START						\
	__asm(							\
	".import $global$, data					\n" \
	"	.import ___start, code				\n" \
	"	.text						\n" \
	"	.align	4					\n" \
	"	.export __start, entry				\n" \
	"	.type	__start,@function			\n" \
	"	.label __start					\n" \
	"	.proc						\n" \
	"	.callinfo frame=0, calls			\n" \
	"	.entry						\n" \
	"	bl L$lpc, %r27					\n" \
	"	depi 0, 31, 2, %r27				\n" \
	"L$lpc:  addil L'$global$ - ($PIC_pcrel$0 - 8), %r27	\n" \
	"	ldo R'$global$ - ($PIC_pcrel$0 - 12)(%r1),%r27	\n" \
	"	.call						\n" \
	"	b	___start				\n" \
	"	copy    %r27, %r19				\n" \
	"	.exit						\n" \
	"	.procend")


#define	MD_RCRT0_START						\
	__asm(							\
	".import $global$, data					\n" \
	"	.import ___start, code				\n" \
	"	.text						\n" \
	"	.align	4					\n" \
	"	.export __start, entry				\n" \
	"	.type	__start,@function			\n" \
	"	.label __start					\n" \
	"	.proc						\n" \
	"	.callinfo frame=0, calls			\n" \
	"	.entry						\n" \
	"	copy	%r3, %r1				\n" \
	"	copy	%sp, %r3				\n" \
	"	stwm	%r1, 64+16*4(%sp)			\n" \
	"	stw	%arg0, -36(%r3)				\n" \
	"	bl	1f, %dp					\n" \
	"	depi	0, 31, 2, %dp				\n" \
	"1:	addil	L'$global$ - ($PIC_pcrel$0 - 8), %dp	\n" \
	"	ldo	R'$global$ - ($PIC_pcrel$0 - 12)(%r1), %dp \n" \
	"	bl	1f, %arg2				\n" \
	"	depi	0, 31, 2, %arg2				\n" \
	"1:	addil	L'_DYNAMIC - ($PIC_pcrel$0 - 8), %arg2	\n" \
	"	ldo	R'_DYNAMIC - ($PIC_pcrel$0 - 12)(%r1), %arg2 \n" \
	"	stw	%arg2, -40(%r3)				\n" \
	"	ldw	0(%arg0), %arg0				\n" \
	"	ldo	4(%r3), %arg1				\n" \
	"	ldo	-4(%arg0), %arg0			\n" \
	"	bl	_dl_boot_bind, %rp			\n" \
	"	copy	%dp, %r19				\n" \
	"	ldw	-36(%r3), %arg0				\n" \
	"	copy	%r0, %arg1				\n" \
	"	ldo	64(%r3), %sp				\n" \
	"	ldwm	-64(%sp), %r3				\n" \
	"	.call						\n" \
	"	b	___start				\n" \
	"	copy	%dp, %r19				\n" \
	"	.exit						\n" \
	"	.procend					\n" \
	"	.export _dl_exit, entry				\n" \
	"	.type	_dl_exit,@function			\n" \
	"	.label _dl_exit					\n" \
	"	.proc						\n" \
	"	.callinfo frame=0, calls			\n" \
	"	.entry						\n" \
	"_dl_exit:						\n" \
	"	stw	%rp, -24(%sp)				\n" \
	"	ldil	L%0xc0000000, %r1			\n" \
	"	ble	4(%sr7, %r1)				\n" \
	"	ldi	1, %t1					\n" \
	"	comb,<>	%r0, %t1, 1f				\n" \
	"	ldw	-24(%sp), %rp				\n" \
	"	bv	%r0(%rp)				\n" \
	"	nop						\n" \
	"1:	bv	%r0(%rp)				\n" \
	"	sub	%r0, %ret0, %ret0			\n" \
	"	.exit						\n" \
	"	.procend					\n" \
	"	.export _dl_printf, entry			\n" \
	"	.type	_dl_printf,@function			\n" \
	"	.label _dl_printf				\n" \
	"	.proc						\n" \
	"	.callinfo frame=0, calls			\n" \
	"	.entry						\n" \
	"_dl_printf:						\n" \
	"	bv	%r0(%rp)				\n" \
	"	nop						\n" \
	"	.exit						\n" \
	"	.procend")


#define	MD_START_ARGS	struct ps_strings *arginfo, void (*cleanup)(void)
#define	MD_START_SETUP				\
	char	**argv, **envp;			\
	int	argc;				\
						\
	argv = arginfo->ps_argvstr;		\
	argc = arginfo->ps_nargvstr;		\
	environ = envp = arginfo->ps_envstr;

#define	MD_EPROL_LABEL	__asm (".export _eprol, entry\n\t.label _eprol")

#include <sys/syscall.h>
#include <machine/vmparam.h>	/* SYSCALLGATE */
#define	MD_DISABLE_KBIND						\
do {									\
	register long r1 __asm__("r1") = SYSCALLGATE;			\
	register void *arg0 __asm__("r26") = NULL;			\
	__asm__ __volatile__ ("ble 4(%%sr7, %%r1) ! ldi %0, %%r22"	\
	    :								\
	    : "i" (SYS_kbind), "r" (r1), "r"(arg0)			\
	    : "r22", "r28", "r29", "cc", "memory");			\
} while (0)
