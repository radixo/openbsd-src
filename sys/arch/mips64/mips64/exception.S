/*	$OpenBSD: exception.S,v 1.37 2014/03/11 07:50:49 jasper Exp $ */

/*
 * Copyright (c) 2002-2003 Opsycon AB  (www.opsycon.se / www.opsycon.com)
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
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 */

/*
 *  This code handles exceptions and dispatches to the
 *  correct handler depending on the exception type.
 *
 *  Exceptions are directed to the following addresses:
 *      0xffffffffbfc00000  Reset, NMI etc. Not handled by the kernel.
 *	0xffffffff80000000  TLB refill, not in exception.
 *	0xffffffff80000080  XTLB refill, not in exception.
 *	0xffffffffa0000100  Cache errors.
 *	0xffffffff80000180  Interrupts. Same as next.
 *	0xffffffff80000180  Everything else...
 */

#include <machine/param.h>
#include <machine/asm.h>
#include <machine/cpu.h>
#include <mips64/mips_cpu.h>
#include <machine/regnum.h>
#include <machine/cpustate.h>
#ifdef CPU_LOONGSON2
#include <machine/loongson2.h>
#endif

#include "assym.h"

	.set	mips3

	.text

k_exception_table:
	PTR_VAL	k_intr			/* T_INT */
	PTR_VAL	k_general		/* T_TLB_MOD */
	PTR_VAL	k_tlb_inv		/* T_TLB_LD_MISS */
	PTR_VAL	k_tlb_inv		/* T_TLB_ST_MISS */
	PTR_VAL	k_general		/* T_ADDR_ERR_LD */
	PTR_VAL	k_general		/* T_ADDR_ERR_ST */
	PTR_VAL	k_general		/* T_BUS_ERR_IFETCH */
	PTR_VAL	k_general		/* T_BUS_ERR_LD_ST */
	PTR_VAL	k_general		/* T_SYSCALL */
	PTR_VAL	k_general		/* T_BREAK */
	PTR_VAL	k_general		/* T_RES_INST */
	PTR_VAL	k_general		/* T_COP_UNUSABLE */
	PTR_VAL	k_general		/* T_OVFLOW */
	PTR_VAL	k_general		/* T_TRAP */
#ifdef CPU_R4000
	PTR_VAL	vcei			/* T_VCEI */
#else
	PTR_VAL	k_general		/* T_VCEI */
#endif
	PTR_VAL	k_general		/* T_FPE */
	PTR_VAL	k_general		/* T_IWATCH */
	PTR_VAL	k_general
	PTR_VAL	k_general		/* T_C2E */
	PTR_VAL	k_general
	PTR_VAL	k_general
	PTR_VAL	k_general
	PTR_VAL	k_general		/* T_MDMX */
	PTR_VAL	k_general		/* T_DWATCH */
	PTR_VAL	k_general		/* T_MCHECK */
	PTR_VAL	k_general
	PTR_VAL	k_general
	PTR_VAL	k_general
	PTR_VAL	k_general
	PTR_VAL	k_general
	PTR_VAL	k_general		/* T_CACHEERR */
#ifdef CPU_R4000
	PTR_VAL	vced			/* T_VCED */
#else
	PTR_VAL	k_general		/* T_VCED */
#endif

u_exception_table:
	PTR_VAL	u_intr			/* T_INT */
	PTR_VAL	u_general		/* T_TLB_MOD */
	PTR_VAL	u_general		/* T_TLB_LD_MISS */
	PTR_VAL	u_general		/* T_TLB_ST_MISS */
	PTR_VAL	u_general		/* T_ADDR_ERR_LD */
	PTR_VAL	u_general		/* T_ADDR_ERR_ST */
	PTR_VAL	u_general		/* T_BUS_ERR_IFETCH */
	PTR_VAL	u_general		/* T_BUS_ERR_LD_ST */
	PTR_VAL	u_general		/* T_SYSCALL */
	PTR_VAL	u_general		/* T_BREAK */
	PTR_VAL	u_general		/* T_RES_INST */
	PTR_VAL	u_general		/* T_COP_UNUSABLE */
	PTR_VAL	u_general		/* T_OVFLOW */
	PTR_VAL	u_general		/* T_TRAP */
#ifdef CPU_R4000
	PTR_VAL	vcei			/* T_VCEI */
#else
	PTR_VAL	u_general		/* T_VCEI */
#endif
	PTR_VAL	u_general		/* T_FPE */
	PTR_VAL	u_general		/* T_IWATCH */
	PTR_VAL	u_general
	PTR_VAL	u_general		/* T_C2E */
	PTR_VAL	u_general
	PTR_VAL	u_general
	PTR_VAL	u_general
	PTR_VAL	u_general		/* T_MDMX */
	PTR_VAL	u_general		/* T_DWATCH */
	PTR_VAL	u_general		/* T_MCHECK */
	PTR_VAL	u_general
	PTR_VAL	u_general
	PTR_VAL	u_general
	PTR_VAL	u_general
	PTR_VAL	u_general
	PTR_VAL	u_general		/* T_CACHEERR */
#ifdef CPU_R4000
	PTR_VAL	vced			/* T_VCED */
#else
	PTR_VAL	u_general		/* T_VCED */
#endif

	.set	noreorder		# Noreorder is default style!

/*---------------------------------------------------------------- exception
 *	General exception handler dispatcher. This code is copied
 *	to the vector area and must thus be PIC and less than 128
 *	bytes long to fit. Only k0 and k1 may be used at this time.
 */
	.globl	cache_err
cache_err:
#ifdef CPU_R4000
	nop
#endif
	.globl	exception
exception:
	.set	noat
#ifdef CPU_LOONGSON2
	/*
	 * To work around a branch prediction issue on earlier LS2F
	 * chips, it is necessary to clear the BTB upon
	 * userland->kernel boundaries.
	 */
	li	k0, COP_0_DIAG_BTB_CLEAR | COP_0_DIAG_RAS_DISABLE
	dmtc0	k0, COP_0_DIAG
#endif
	MFC0	k0, COP_0_STATUS_REG
	MFC0	k1, COP_0_CAUSE_REG
	and	k0, k0, SR_KSU_USER
	beqz	k0, k_exception		# Kernel mode mode
	and	k1, k1, CR_EXC_CODE

	LA	k0, u_exception_table
	PTR_ADDU k0, k0, k1
	PTR_ADDU k0, k0, k1		# yes, twice...
	PTR_L	k0, 0(k0)
	j	k0
	nop

k_exception:
	LA	k0, k_exception_table
	PTR_ADDU k0, k0, k1
	PTR_ADDU k0, k0, k1		# yes, twice...
	PTR_L	k0, 0(k0)
	j	k0
	nop
	.set	at
	.globl	e_exception
e_exception:


/*---------------------------------------------------------------- k_intr
 *	Handle an interrupt in kernel mode. This is easy since we
 *	just need to save away the 'save' registers and state.
 *	State is saved on kernel stack.
 */

NNON_LEAF(k_intr, FRAMESZ(KERN_EXC_FRAME_SIZE), ra)
	.set	noat
	.mask	0x80000000, (CF_RA_OFFS - FRAMESZ(KERN_EXC_FRAME_SIZE))
	PTR_SUB	k0, sp, FRAMESZ(KERN_EXC_FRAME_SIZE)
	SAVE_CPU(k0, CF_RA_OFFS)
#ifdef RM7000_ICR
	cfc0	v1, COP_0_ICR
	SAVE_REG(v1, IC, k0, CF_RA_OFFS)
#endif
	.set	at
	move	sp, k0			# Already on kernel stack
	LA	gp, _gp
	and	t0, a1, ~(SR_COP_1_BIT | SR_EXL | SR_INT_ENAB | SR_KSU_MASK)
	MTC0	t0, COP_0_STATUS_REG
	MTC0_SR_IE_HAZARD
	PTR_S	a0, 0(sp)
	jal	interrupt
	PTR_S	a3, CF_RA_OFFS + KERN_REG_SIZE(sp)

	PTR_L	a0, CF_RA_OFFS + KERN_REG_SIZE(sp)
	.set	noat
#ifdef RM7000_ICR
	RESTORE_REG(t0, IC, sp, CF_RA_OFFS)
	ctc0	t0, COP_0_ICR
#endif
	RESTORE_CPU(sp, CF_RA_OFFS)
	PTR_ADDU sp, sp, FRAMESZ(KERN_EXC_FRAME_SIZE)
	ERET
	.set	at
END(k_intr)

/*---------------------------------------------------------------- u_intr
 *	Handle an interrupt in user mode. Save the relevant user
 *	registers into the u.u_pcb struct. This will allow us
 *	to preempt the interrupted process. Full save is held
 *	off though until a switch() really is required.
 */
NNON_LEAF(u_intr, FRAMESZ(CF_SZ), ra)
	.set	noat
	.mask	0x80000000, (CF_RA_OFFS - FRAMESZ(CF_SZ))
	GET_CPU_INFO(k1, k0)
	PTR_L	k0, CI_CURPROCPADDR(k1)
	SAVE_CPU(k0, 0)
#ifdef RM7000_ICR
	cfc0	v1, COP_0_ICR
	SAVE_REG(v1, IC, k0, 0)
#endif
	PTR_ADDU sp, k0, USPACE-FRAMESZ(CF_SZ)
	LA	gp, _gp
	.set	at
	and	t0, a1, ~(SR_COP_1_BIT | SR_EXL | SR_INT_ENAB | SR_KSU_MASK)
	MTC0	t0, COP_0_STATUS_REG
	MTC0_SR_IE_HAZARD
	PTR_S	a0, 0(sp)
	jal	interrupt
	PTR_S	a3, CF_RA_OFFS(sp)	# for debugging

	MFC0	t0, COP_0_STATUS_REG	# enable interrupts before checking
	ori	t0, SR_INT_ENAB		# for ast.
	MTC0	t0, COP_0_STATUS_REG
	MTC0_SR_IE_HAZARD

	GET_CPU_INFO(t1, t0)
0:
	PTR_L	t0, CI_CURPROC(t1)
	lw	v0, P_ASTPENDING(t0)	# any pending AST?
	beq	v0, zero, 4f
	nop

	PTR_L	t0, CI_CURPROCPADDR(t1)	# curprocpaddr
	SAVE_CPU_SREG(t0, 0)

#ifdef PERFCNTRS
	lw	t0, cpu_is_rm7k
	beqz	t0, 1f			# not an RM7K. Don't do perf save.
	nop

	mfc0	v0, COP_0_PC_CTRL
	PTR_L	t0, curproc
	sw	v0, P_PC_CTRL(t0)
	dmfc0	v0, COP_0_WATCH_1
	dmfc0	v1, COP_0_WATCH_2
	sd	v0, P_WATCH_1(t0)
	sd	v1, P_WATCH_2(t0)
	mfc0	v0, COP_0_WATCH_M
	mfc0	v1, COP_0_PC_COUNT
	sw	v0, P_WATCH_M(t0)
	sw	v1, P_PC_COUNT(t0)
	mtc0	zero, COP_0_PC_CTRL
	dmtc0	zero, COP_0_WATCH_1
	dmtc0	zero, COP_0_WATCH_2
	nop;nop;nop;nop
1:
#endif
	jal	ast
	nop

/*
 * Restore user registers and return. NOTE: interrupts are enabled.
 */
#ifdef PERFCNTRS
	lw	t0, cpu_is_rm7k
	beqz	t0, 1f			# not an RM7K. Don't do perf setup.

	GET_CPU_INFO(t1, t0)
	PTR_L	t0, CI_CURPROC(t1)	# set up rm7k.
	ld	v0, P_WATCH_1(t1)
	dmtc0	v0, COP_0_WATCH_1
	ld	v0, P_WATCH_2(t1)
	dmtc0	v0, COP_0_WATCH_2
	lw	v0, P_WATCH_M(t1)
	mtc0	v0, COP_0_WATCH_M
	lw	v0, P_PC_CTRL(t1)
	lw	v1, P_PC_COUNT(t1)
	nop;nop
	mtc0	v0, COP_0_PC_CTRL
	nop;nop;nop;nop
	mtc0	v1, COP_0_PC_COUNT
	nop;nop;nop;nop
1:
#endif
	GET_CPU_INFO(t1, t0)
	PTR_L	t0, CI_CURPROCPADDR(t1)
	RESTORE_CPU_SREG(t0, 0)

	b	0b
	nop

4:
	MFC0	t0, COP_0_STATUS_REG	# disable interrupts
	LI	v0, ~SR_INT_ENAB
	and	t0, t0, v0
	MTC0	t0, COP_0_STATUS_REG
	MTC0_SR_IE_HAZARD

	ori	t0, SR_EXL		# restoring to user mode.
	MTC0	t0, COP_0_STATUS_REG	# must set exception level bit.
	MTC0_SR_IE_HAZARD

	# t1 is curcpu() from earlier
	move	k1, t1
	PTR_L	k0, CI_CURPROCPADDR(k1)
	RESTORE_REG(a3, CPL, k0, 0)
	sw	a3, CI_IPL(k1)
	.set	noat
	RESTORE_REG(a0, PC, k0, 0)
#ifdef RM7000_ICR
	RESTORE_REG(t0, IC, k0, 0)
	ctc0	t0, COP_0_ICR
#endif
	RESTORE_CPU(k0, 0)
	RESTORE_REG(sp, SP, k0, 0)
	LI	k0, 0
	LI	k1, 0
	ERET
	.set	at
END(u_intr)

/*---------------------------------------------------------------- k_general
 *	Handle a kernel general trap. This is very much like
 *	k_intr except that we call ktrap instead of interrupt.
 */

NNON_LEAF(k_general, FRAMESZ(KERN_EXC_FRAME_SIZE), ra)
	.set	noat
	.mask	0x80000000, (CF_RA_OFFS - FRAMESZ(KERN_EXC_FRAME_SIZE))
	PTR_SUB	k0, sp, FRAMESZ(KERN_EXC_FRAME_SIZE)
	SAVE_CPU(k0, CF_RA_OFFS)
#ifdef RM7000_ICR
	cfc0	v1, COP_0_ICR
	SAVE_REG(v1, IC, k0, CF_RA_OFFS)
#endif
#if defined(DDB)
	SAVE_CPU_SREG(k0, CF_RA_OFFS)
#endif
	.set	at
	move	sp, k0			# Already on kernel stack
	LA	gp, _gp
	and	t0, a1, ~(SR_COP_1_BIT | SR_EXL | SR_INT_ENAB | SR_KSU_MASK)
	MTC0	t0, COP_0_STATUS_REG
	MTC0_SR_IE_HAZARD
	PTR_S	a0, 0(sp)
	jal	trap
	PTR_S	a3, CF_RA_OFFS + KERN_REG_SIZE(sp)

	MFC0	t0, COP_0_STATUS_REG	# disable interrupts
	LI	t1, ~SR_INT_ENAB
	and	t0, t0, t1
	MTC0	t0, COP_0_STATUS_REG
	MTC0_SR_IE_HAZARD

	.set	noat
#ifdef RM7000_ICR
	RESTORE_REG(t0, IC, sp, CF_RA_OFFS)
	ctc0	t0, COP_0_ICR
#endif
	RESTORE_REG(a0, PC, sp, CF_RA_OFFS)
	RESTORE_CPU(sp, CF_RA_OFFS)
	PTR_ADDU sp, sp, FRAMESZ(KERN_EXC_FRAME_SIZE)
	ERET
	.set	at
END(k_general)

/*---------------------------------------------------------------- u_general
 *	Handle a user general trap.
 */
NNON_LEAF(u_general, FRAMESZ(CF_SZ), ra)
	.set	noat
	.mask	0x80000000, (CF_RA_OFFS - FRAMESZ(CF_SZ))

	GET_CPU_INFO(k1, k0)
	PTR_L	k0, CI_CURPROCPADDR(k1)
	SAVE_CPU(k0, 0)
#ifdef RM7000_ICR
	cfc0	v1, COP_0_ICR
	SAVE_REG(v1, IC, k0, 0)
#endif
	SAVE_CPU_SREG(k0, 0)
	PTR_ADDU sp, k0, USPACE-FRAMESZ(CF_SZ)
	LA	gp, _gp
	.set	at
	and	t0, a1, ~(SR_COP_1_BIT | SR_EXL | SR_INT_ENAB | SR_KSU_MASK)
	MTC0	t0, COP_0_STATUS_REG
	MTC0_SR_IE_HAZARD

#ifdef PERFCNTRS
	lw	t0, cpu_is_rm7k
	beqz	t0, 1f			# not an RM7K. Don't do perf save.
	nop

	mfc0	v0, COP_0_PC_CTRL
	PTR_L	t0, CI_CURPROC(k1)
	sw	v0, P_PC_CTRL(t0)
	dmfc0	v0, COP_0_WATCH_1
	dmfc0	v1, COP_0_WATCH_2
	sd	v0, P_WATCH_1(t0)
	sd	v1, P_WATCH_2(t0)
	mfc0	v0, COP_0_WATCH_M
	mfc0	v1, COP_0_PC_COUNT
	sw	v0, P_WATCH_M(t0)
	sw	v1, P_PC_COUNT(t0)
	mtc0	zero, COP_0_PC_CTRL
	nop;nop;nop;nop
1:
#endif

	jal	trap
	PTR_S	a3, CF_RA_OFFS(sp)		# for debugging

0:
	GET_CPU_INFO(t1, t0)
	PTR_L	t0, CI_CURPROC(t1)
	lw	v0, P_ASTPENDING(t0)	# any pending AST?
	beq	v0, zero, 4f
	nop

	jal	ast
	nop

	b	0b
	nop

4:
#ifdef PERFCNTRS
	lw	t0, cpu_is_rm7k
	beqz	t0, 1f			# not an RM7K. Don't do perf setup.

	GET_CPU_INFO(t1, t0)
	PTR_L	t0, CI_CURPROC(k1)	# set up rm7k.
	ld	v0, P_WATCH_1(t0)
	dmtc0	v0, COP_0_WATCH_1
	ld	v0, P_WATCH_2(t0)
	dmtc0	v0, COP_0_WATCH_2
	lw	v0, P_WATCH_M(t0)
	mtc0	v0, COP_0_WATCH_M
	lw	v0, P_PC_CTRL(t0)
	lw	v1, P_PC_COUNT(t0)
	nop;nop
	mtc0	v0, COP_0_PC_CTRL
	nop;nop;nop;nop
	mtc0	v1, COP_0_PC_COUNT
	nop;nop;nop;nop
1:
#endif
	MFC0	t0, COP_0_STATUS_REG	# disable interrupts
	LI	t1, ~SR_INT_ENAB
	and	t0, t0, t1
	MTC0	t0, COP_0_STATUS_REG
	MTC0_SR_IE_HAZARD

	ori	t0, SR_EXL		# restoring to user mode.
	MTC0	t0, COP_0_STATUS_REG	# must set exception level bit.
	MTC0_SR_IE_HAZARD

	GET_CPU_INFO(k1, k0)
	PTR_L	k0, CI_CURPROCPADDR(k1)
	RESTORE_REG(a3, CPL, k0, 0)
	sw	a3, CI_IPL(k1)
	.set	noat
	RESTORE_CPU_SREG(k0, 0)
	RESTORE_REG(a0, PC, k0, 0)
#ifdef RM7000_ICR
	RESTORE_REG(t0, IC, k0, 0)
	ctc0	t0, COP_0_ICR
#endif
	RESTORE_CPU(k0, 0)
	RESTORE_REG(sp, SP, k0, 0)
	LI	k0, 0
	LI	k1, 0
	ERET
	.set	at
END(u_general)

#ifdef CPU_R4000
LEAF(vced, 0)
	.set	noat
	dmfc0	k0, COP_0_BAD_VADDR
	ori	k0, k0, 3		# need to align badvaddr...
	xori	k0, k0, 3		# ...to a word boundary.
	cache	0x17, 0(k0)		# HitWBInvalidate_S
	cache	0x11, 0(k0)		# HitWBInvalidate_D
	LI	k0, 0
	ERET
	.set	at
END(vced)

LEAF(vcei, 0)
	.set	noat
	dmfc0	k0, COP_0_BAD_VADDR
	cache	0x17, 0(k0)		# HitWBInvalidate_S
	cache	0x10, 0(k0)		# HitInvalidate_I
	LI	k0, 0
	ERET
	.set	at
END(vcei)
#endif
