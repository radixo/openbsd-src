#	$OpenBSD: Makefile,v 1.7 2012/09/02 03:56:18 deraadt Exp $

PROG=	tic
SRCS=	dump_entry.c tic.c
MAN=	tic.1 captoinfo.1 infotocap.1
LINKS=	${BINDIR}/tic ${BINDIR}/infotocap ${BINDIR}/tic ${BINDIR}/captoinfo
DPADD=	${LIBCURSES}
LDADD=	-lcurses
CURSES=	${.CURDIR}/../../lib/libcurses
CFLAGS+= -I${CURSES} -I${.CURDIR} -I.
CLEANFILES+= termsort.c captoinfo.1

# Work around gcc optimization bug on sh
.if (${MACHINE_ARCH} == sh)
COPTS+=	-O0
.endif

beforedepend: termsort.c

termsort.c: MKtermsort.sh
	sh ${.CURDIR}/MKtermsort.sh awk ${CURSES}/Caps > ${.TARGET}

captoinfo.1: captoinfo.1tbl
	cp ${.ALLSRC} ${.TARGET}

.include <bsd.prog.mk>
