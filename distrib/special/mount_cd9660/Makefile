#	$OpenBSD: Makefile,v 1.3 1997/09/21 11:37:09 deraadt Exp $

PROG=	mount_cd9660
SRCS=	mount_cd9660.c getmntopts.c
MAN=	mount_cd9660.8

MOUNT=	${.CURDIR}/../../../sbin/mount
CFLAGS+= -I${MOUNT}
.PATH:	${MOUNT}

.PATH:  ${.CURDIR}/../../../sbin/mount_cd9660
.include <bsd.prog.mk>
