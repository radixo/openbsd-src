# $OpenBSD: Makefile,v 1.1.1.1 2005/09/15 15:37:15 kurt Exp $

.include <bsd.obj.mk>

AA_DIR=${.CURDIR}/../libaa

AA_OBJDIR!=	if [ -d $(AA_DIR)/${__objdir} ]; then \
			echo "$(AA_DIR)/${__objdir}"; \
		else \
			echo "$(AA_DIR)"; \
		fi

PROG=		prog2
SRCS=		main.c
CPPFLAGS+= 	-I$(AA_DIR)
LDADD+=		-laa
LDFLAGS+=	-Wl,-E
LDFLAGS+=	-Wl,-rpath,$(AA_OBJDIR)
LDFLAGS+=	-L$(AA_OBJDIR)

.include <bsd.regress.mk>
