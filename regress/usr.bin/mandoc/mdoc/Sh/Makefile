# $OpenBSD: Makefile,v 1.5 2014/11/25 03:04:32 schwarze Exp $

REGRESS_TARGETS	 = badNAME before empty emptyNAME first nohead order subbefore
LINT_TARGETS	 = badNAME before empty emptyNAME first nohead order subbefore

SKIP_GROFF	= subbefore first empty
SKIP_ASCII	= first

.include <bsd.regress.mk>
