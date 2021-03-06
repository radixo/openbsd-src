/*	$OpenBSD: rtld_machine.c,v 1.55 2015/09/23 22:52:12 kettenis Exp $ */

/*
 * Copyright (c) 1999 Dale Rahn
 * Copyright (c) 2001 Niklas Hallqvist
 * Copyright (c) 2001 Artur Grabowski
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

#define _DYN_LOADER

#include <sys/types.h>
#include <sys/mman.h>
#include <sys/exec.h>
#include <sys/syscall.h>
#include <sys/unistd.h>
#include <machine/pal.h>

#include <nlist.h>
#include <link.h>

#include "syscall.h"
#include "archdep.h"
#include "resolve.h"

#define	DT_PROC(n)	((n) - DT_LOPROC + DT_NUM)

int64_t pcookie __attribute__((section(".openbsd.randomdata"))) __dso_hidden;

int
_dl_md_reloc(elf_object_t *object, int rel, int relasz)
{
	long	i;
	long	numrela;
	long	relrel;
	int	fails = 0;
	Elf64_Addr loff;
	Elf64_Addr prev_value = 0;
	const Elf_Sym *prev_sym = NULL;
	Elf64_Rela  *relas;
	struct load_list *llist;

	loff = object->obj_base;
	numrela = object->Dyn.info[relasz] / sizeof(Elf64_Rela);
	relrel = rel == DT_RELA ? object->relacount : 0;
	relas = (Elf64_Rela *)(object->Dyn.info[rel]);

	if (relas == NULL)
		return(0);

	if (relrel > numrela) {
		_dl_printf("relacount > numrel: %ld > %ld\n", relrel, numrela);
		_dl_exit(20);
	}

	/*
	 * unprotect some segments if we need it.
	 * XXX - we unprotect way to much. only the text can have cow
	 * relocations.
	 */
	if ((object->dyn.textrel == 1) && (rel == DT_REL || rel == DT_RELA)) {
		for (llist = object->load_list; llist != NULL; llist = llist->next) {
			if (!(llist->prot & PROT_WRITE)) {
				_dl_mprotect(llist->start, llist->size,
				    llist->prot|PROT_WRITE);
			}
		}
	}

	/* tight loop for leading RELATIVE relocs */
	for (i = 0; i < relrel; i++, relas++) {
		Elf_Addr *r_addr;

#ifdef DEBUG
		if (ELF64_R_TYPE(relas->r_info) != R_TYPE(RELATIVE)) {
			_dl_printf("RELACOUNT wrong\n");
			_dl_exit(20);
		}
#endif

		r_addr = (Elf64_Addr *)(relas->r_offset + loff);

		/* Handle unaligned RELATIVE relocs */
		if ((((Elf_Addr)r_addr) & 0x7) != 0) {
			Elf_Addr tmp;
			_dl_bcopy(r_addr, &tmp, sizeof(Elf_Addr));
			tmp += loff;
			_dl_bcopy(&tmp, r_addr, sizeof(Elf_Addr));
		} else
			*r_addr += loff;
	}
	for (; i < numrela; i++, relas++) {
		Elf64_Addr *r_addr;
		Elf64_Addr ooff;
		const Elf64_Sym *sym, *this;
		const char *symn;

		r_addr = (Elf64_Addr *)(relas->r_offset + loff);

		if (ELF64_R_SYM(relas->r_info) == 0xffffffff)
			continue;


		sym = object->dyn.symtab;
		sym += ELF64_R_SYM(relas->r_info);
		symn = object->dyn.strtab + sym->st_name;

		this = NULL;
		switch (ELF64_R_TYPE(relas->r_info)) {
		case R_TYPE(REFQUAD):
			ooff =  _dl_find_symbol_bysym(object,
			    ELF64_R_SYM(relas->r_info), &this,
			    SYM_SEARCH_ALL|SYM_WARNNOTFOUND|SYM_NOTPLT,
			    sym, NULL);
			if (this == NULL)
				goto resolve_failed;
			*r_addr += ooff + this->st_value + relas->r_addend;
			break;
		case R_TYPE(RELATIVE):
			/*
			 * There is a lot of unaligned RELATIVE
			 * relocs generated by gcc in the exception handlers.
			 */
			if ((((Elf_Addr) r_addr) & 0x7) != 0) {
				Elf_Addr tmp;
#if 0
_dl_printf("unaligned RELATIVE: %p type: %d %s 0x%lx -> 0x%lx\n", r_addr,
    ELF_R_TYPE(relas->r_info), object->load_name, *r_addr, *r_addr+loff);
#endif
				_dl_bcopy(r_addr, &tmp, sizeof(Elf_Addr));
				tmp += loff;
				_dl_bcopy(&tmp, r_addr, sizeof(Elf_Addr));
			} else
				*r_addr += loff;
			break;
		case R_TYPE(JMP_SLOT):
			ooff = _dl_find_symbol(symn, &this,
			    SYM_SEARCH_ALL|SYM_WARNNOTFOUND|SYM_PLT,
			    sym, object, NULL);
			if (this == NULL)
				goto resolve_failed;
			*r_addr = ooff + this->st_value + relas->r_addend;
			break;
		case R_TYPE(GLOB_DAT):
			if (sym == prev_sym) {
				*r_addr = prev_value + relas->r_addend;
				break;
			}
			ooff =  _dl_find_symbol_bysym(object,
			    ELF64_R_SYM(relas->r_info), &this,
			    SYM_SEARCH_ALL|SYM_WARNNOTFOUND|SYM_NOTPLT,
			    sym, NULL);
			if (this == NULL)
				goto resolve_failed;
			prev_sym = sym;
			prev_value = ooff + this->st_value;
			*r_addr = prev_value + relas->r_addend;
			break;
		case R_TYPE(NONE):
			break;
		default:
			_dl_printf("%s:"
			    " %s: unsupported relocation '%s' %d at %lx\n",
			    _dl_progname, object->load_name, symn,
			    ELF64_R_TYPE(relas->r_info), r_addr );
			_dl_exit(1);
		}
		continue;
resolve_failed:
		if (ELF_ST_BIND(sym->st_info) != STB_WEAK)
			fails++;
	}
	__asm volatile("imb" : : : "memory");

	/* reprotect the unprotected segments */
	if ((object->dyn.textrel == 1) && (rel == DT_REL || rel == DT_RELA)) {
		for (llist = object->load_list; llist != NULL; llist = llist->next) {
			if (!(llist->prot & PROT_WRITE))
				_dl_mprotect(llist->start, llist->size,
				    llist->prot);
		}
	}
	return (fails);
}

/*
 * Resolve a symbol at run-time.
 */
Elf_Addr
_dl_bind(elf_object_t *object, int reloff)
{
	Elf_RelA *rela;
	Elf_Addr ooff;
	const Elf_Sym *sym, *this;
	const char *symn;
	const elf_object_t *sobj;
	uint64_t cookie = pcookie;
	struct {
		struct __kbind param;
		Elf_Addr newval;
	} buf;

	rela = (Elf_RelA *)(object->Dyn.info[DT_JMPREL] + reloff);

	sym = object->dyn.symtab;
	sym += ELF64_R_SYM(rela->r_info);
	symn = object->dyn.strtab + sym->st_name;

	this = NULL;
	ooff = _dl_find_symbol(symn, &this,
	    SYM_SEARCH_ALL|SYM_WARNNOTFOUND|SYM_PLT, sym, object, &sobj);
	if (this == NULL) {
		_dl_printf("lazy binding failed!\n");
		*(volatile int *)0 = 0;		/* XXX */
	}

	buf.newval = ooff + this->st_value + rela->r_addend;

	if (__predict_false(sobj->traced) && _dl_trace_plt(sobj, symn))
		return (buf.newval);

	buf.param.kb_addr = (Elf_Addr *)(object->obj_base + rela->r_offset);
	buf.param.kb_size = sizeof(Elf_Addr);

	/* directly code the syscall, so that it's actually inline here */
	{
		register long syscall_num __asm("$0") /* v0 */ = SYS_kbind;
		register void *arg1 __asm("$16") /* a0 */ = &buf;
		register long  arg2 __asm("$17") /* a1 */ = sizeof(buf);
		register long  arg3 __asm("$18") /* a2 */ = cookie;

		__asm volatile( "call_pal %1" : "+r" (syscall_num)
		    : "i" (PAL_OSF1_callsys), "r" (arg1), "r" (arg2),
		    "r" (arg3) : "$19", "$20", "memory");
	}

	return (buf.newval);
}

/*
 *	Relocate the Global Offset Table (GOT).
 */
int
_dl_md_reloc_got(elf_object_t *object, int lazy)
{
	int	fails = 0;
	Elf_Addr *pltgot;
	extern void _dl_bind_start(void);	/* XXX */
	extern void _dl_bind_secureplt(void);	/* XXX */
	Elf_Addr seg_start;
	u_long pltro;

	if (object->Dyn.info[DT_PLTREL] != DT_RELA)
		return (0);

	pltro = object->Dyn.info[DT_PROC(DT_ALPHA_PLTRO)];
	pltgot = (Elf_Addr *)object->Dyn.info[DT_PLTGOT];

	if (object->traced)
		lazy = 1;

	if (object->obj_type == OBJTYPE_LDR || !lazy || pltgot == NULL) {
		fails = _dl_md_reloc(object, DT_JMPREL, DT_PLTRELSZ);
	} else {
		if (object->obj_base != 0) {
			int i, size;
			Elf_Addr *addr;
			Elf_RelA *rela;

			size = object->Dyn.info[DT_PLTRELSZ] /
			    sizeof(Elf_RelA);
			rela = (Elf_RelA *)(object->Dyn.info[DT_JMPREL]);

			for (i = 0; i < size; i++) {
				addr = (Elf_Addr *)(object->obj_base +
				    rela[i].r_offset);
				*addr += object->obj_base;
			}
		}
		if (pltro == 0) {
			pltgot[2] = (Elf_Addr)_dl_bind_start;
			pltgot[3] = (Elf_Addr)object;
		} else {
			pltgot[0] = (Elf_Addr)_dl_bind_secureplt;
			pltgot[1] = (Elf_Addr)object;
		}
	}

	/* mprotect the GOT */
	seg_start = 0;
	if (pltro != 0)
		seg_start = (Elf_Addr)pltgot;
	_dl_protect_segment(object, seg_start, "__got_start", "__got_end",
	    PROT_READ);

	/* mprotect the PLT, if it isn't already read-only */
	if (pltro == 0)
		_dl_protect_segment(object, (Elf_Addr)pltgot, "__plt_start",
		    "__plt_end", PROT_READ|PROT_EXEC);

	return (fails);
}

/* relocate the GOT early */

void	_reloc_alpha_got(Elf_Dyn *dynp, Elf_Addr relocbase);

void
_reloc_alpha_got(Elf_Dyn *dynp, Elf_Addr relocbase)
{
	const Elf_RelA *rela = 0, *relalim;
	Elf_Addr relasz = 0;
	Elf_Addr *where;

	for (; dynp->d_tag != DT_NULL; dynp++) {
		switch (dynp->d_tag) {
		case DT_RELA:
			rela = (const Elf_RelA *)(relocbase + dynp->d_un.d_ptr);
			break;
		case DT_RELASZ:
			relasz = dynp->d_un.d_val;
			break;
		}
	}
	relalim = (const Elf_RelA *)((caddr_t)rela + relasz);
	for (; rela < relalim; rela++) {
		if (ELF64_R_TYPE(rela->r_info) != RELOC_RELATIVE)
			continue;
		where = (Elf_Addr *)(relocbase + rela->r_offset);
		*where += (Elf_Addr)relocbase;
	}
}
