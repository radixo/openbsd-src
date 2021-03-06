#	$OpenBSD: Makefile,v 1.59 2015/08/28 14:16:01 kettenis Exp $

SUBDIR=ldconfig ldd
MAN=	ld.so.1

.include <bsd.own.mk>

.if defined(NOPIC)
NOPROG=
.else
PROG=	ld.so
BINMODE=444

VERSION_SCRIPT=	${.CURDIR}/Symbols.map
ELF_LDFLAGS+=--version-script=${VERSION_SCRIPT}

VPATH=${.CURDIR}/../../lib/libc/string

SRCS=	ldasm.S boot.c loader.c resolve.c dlfcn.c dl_printf.c rtld_machine.c
SRCS+=	path.c util.c sod.c strsep.c strtol.c dir.c library_subr.c dl_prebind.c
SRCS+=	dl_realpath.c dl_uname.c dl_dirname.c strlcat.c strlen.c trace.c
SRCS+=	malloc.c reallocarray.c

.if (${MACHINE_ARCH} == "i386")
SRCS+=	library_mquery.c
.else
SRCS+=	library.c
.endif

.include "${.CURDIR}/${MACHINE_CPU}/Makefile.inc"
.PATH:	${.CURDIR}/${MACHINE_CPU}

DEBUG?=	-g
CFLAGS += -Wall -Werror
CFLAGS += -I${.CURDIR} -I${.CURDIR}/${MACHINE_CPU} \
	-D'DEF_WEAK(x)=asm("")' -D'DEF_STRONG(x)=asm("")' \
	-Dstrsep=_dl_strsep -Dstrlcat=_dl_strlcat -Dstrlen=_dl_strlen
CDIAGFLAGS=
INSTALL_STRIP=

ELF_LDFLAGS+=--shared -Bsymbolic --no-undefined

test_prog=	test-$(PROG)
CLEANFILES+=	test-$(PROG)
candidate=	$(PROG).test
CLEANFILES+=	${candidate}

$(test_prog):
	printf '#include <stdio.h>\nint main(int argc, char **argv){ printf("%%s: ", argv[0]); printf("%%s!\\n", argv[1] ? argv[1] : "foo"); }\n' | \
	$(CC) -x c - -Wl,-dynamic-linker,${.OBJDIR}/$(candidate) -o $@

$(PROG): $(test_prog) ${VERSION_SCRIPT}
	$(LD) -x -e _dl_start $(ELF_LDFLAGS) -o $(candidate) $(OBJS) $(LDADD)
.ifndef CROSSDIR
	ulimit -c 0; [ "`${.OBJDIR}/$(test_prog) ok`" = "${.OBJDIR}/$(test_prog): ok!" ]
.endif
	cp $(candidate) $@
.endif

.include <bsd.prog.mk>

boot.o: boot.c
	${COMPILE.c} -fno-stack-protector ${.IMPSRC}
