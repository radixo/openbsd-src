#	$OpenBSD$

TEST_ENV=      "MALLOC_OPTIONS=AFGJPRX"

PROG=test_hostkeys
SRCS=tests.c test_iterate.c
REGRESS_TARGETS=run-regress-${PROG}

run-regress-${PROG}: ${PROG}
	env ${TEST_ENV} ./${PROG} -d ${.CURDIR}/testdata

.include <bsd.regress.mk>
