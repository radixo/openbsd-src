# $FreeBSD: src/gnu/usr.bin/cc/cc1plus/Makefile,v 1.39.8.1 2009/04/15 03:14:26 kensmith Exp $
# $OpenBSD: Makefile,v 1.1.1.1 2009/10/15 20:50:20 robert Exp $

.include <bsd.own.mk>

.include "${.CURDIR}/../Makefile.inc"
.include "${.CURDIR}/../Makefile.ver"

.PATH: ${GCCDIR}/cp ${GCCDIR}

PROG=	cc1plus
#SRCS=	cfns.h
SRCS+=	main.c cp-lang.c c-opts.c call.c class.c cvt.c cxx-pretty-print.c \
	decl.c decl2.c error.c except.c expr.c dump.c friend.c  init.c lex.c \
	mangle.c method.c name-lookup.c parser.c pt.c ptree.c repo.c rtti.c \
	search.c semantics.c tree.c typeck.c typeck2.c optimize.c \
	cp-objcp-common.c cp-gimplify.c tree-mudflap.c

BINDIR= /usr/lib/gcc-lib/${GCC_TARGET}/${BASEVER}

NOMAN=	Yes

CFLAGS+= -I${GCCDIR}/cp -I.

OBJS+=	${PROG}-checksum.o
DPADD=	${LIBBACKEND} ${LIBCPP} ${LIBDECNUMBER} ${LIBIBERTY}
LDADD=	${LIBBACKEND} ${LIBCPP} ${LIBDECNUMBER} ${LIBIBERTY}

DOBJS+=	${SRCS:N*.h:R:S/$/.o/g}
${PROG}-dummy:	${DOBJS}
	${CC} ${CFLAGS} ${LDFLAGS} -o ${.TARGET} ${DOBJS} ${LDADD}
CLEANFILES+=	${PROG}-dummy

${PROG}-checksum.c:	${PROG}-dummy
	../cc_tools/genchecksum ${PROG}-dummy > ${.TARGET}
CLEANFILES+=	${PROG}-checksum.c

.include <bsd.prog.mk>
