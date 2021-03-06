#	$OpenBSD: Makefile,v 1.15 2014/07/13 09:26:08 jasper Exp $

MAN=	boot.8
MANSUBDIR=zaurus
MLINKS=	boot.8 boot.conf.5

.if ${MACHINE} == "zaurus"
PROG=	zboot
LDFLAGS+=-nostdlib -Bstatic -nopie
INSTALL_STRIP=

SRCS=	crt0.c

S=	${.CURDIR}/../../../..

CLEANFILES+= arm machine
.if !make(libdep) && !make(sadep) && !make(salibdir) && !make(obj)
.BEGIN:
	@([ X$(S) = X -o -h arm ] || ln -s $(S)/arch/arm/include arm)
	@([ X$(S) = X -o -h machine ] || ln -s $(S)/arch/zaurus/include machine)
.endif

# stand/boot
SRCS+=	boot.c cmd.c vars.c bootarg.c conf.c

# libsa
SRCS+=	close.c closeall.c cons.c ctime.c disklabel.c dkcksum.c fstat.c \
	lseek.c memcmp.c memcpy.c memset.c open.c printf.c read.c \
	readdir.c snprintf.c stat.c strerror.c strtol.c ufs.c

.PATH:	${S}/lib/libkern/arch/arm ${S}/lib/libkern
SRCS+=	getchar.c putchar.c strcmp.c strlcpy.c strlen.c strncmp.c strncpy.c \
	write.c ashrdi3.c divsi3.S divdi3.c moddi3.c qdivrem.c

# local overrides and additions
SRCS+=	alloc.c devopen.c diskprobe.c exec.c exit.c loadfile.c \
	machdep.c termios.c unixcons.c unixdev.c unixsys.S

.PATH:	${S}/stand/boot
.PATH:	${S}/lib/libsa

${PROG}: ${OBJS} ${LIBSA}
	${LD} ${LDFLAGS} -o ${PROG} ${OBJS} ${LIBSA}

.else
NOPROG=
.endif

.include <bsd.prog.mk>

AFLAGS+=-D_LOCORE
CPPFLAGS+=-D_STANDALONE
CPPFLAGS+=-I${S}/stand/boot -I${S}/lib/libsa -I. -I${.CURDIR} -I${S}
CFLAGS+=-fno-stack-protector -fno-builtin -fpack-struct -fno-pie
AFLAGS+= -fno-pie
