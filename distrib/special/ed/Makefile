#	$OpenBSD: Makefile,v 1.1 2009/10/28 05:06:37 deraadt Exp $

PROG=	ed
CFLAGS+=-DBACKWARDS
SRCS=	 buf.c glbl.c io.c main.c re.c sub.c undo.c

.PATH:	${.CURDIR}/../../../bin/ed
.include <bsd.prog.mk>
