#
#	$OpenBSD: Makefile.inc,v 1.6 2013/10/15 22:46:51 deraadt Exp $
#

# TOP is assumed to be defined by Makefile including this one.

CBIN?=		instbin
LISTS=		${.CURDIR}/list
UTILS?=		${.CURDIR}/../../miniroot

COMMONDIR=	${TOP}/ramdisk
MOUNT_POINT?=	${TOP}/${BASE}/fs

CRUNCHCONF?=	${COMMONDIR}/${CBIN}.conf
MTREE=		${UTILS}/mtree.conf

${CBIN}.conf: ${LISTS}
	awk -f ${UTILS}/makeconf.awk CBIN=${CBIN} ${LISTS} > ${CBIN}.conf

${CBIN}.mk ${CBIN}.cache ${CBIN}.c: ${CBIN}.conf
	crunchgen -D ${BSDSRCDIR} -L ${DESTDIR}/usr/lib ${CBIN}.conf

${CBIN}: ${CBIN}.mk ${CBIN}.cache ${CBIN}.c
	${MAKE} -f ${CBIN}.mk all

#${CBIN}: ${CBIN}.mk ${CBIN}.cache ${CBIN}.c
#	${MAKE} -f ${CBIN}.mk SRCLIBDIR=${.CURDIR}/../../../lib all

do_files:
	mtree -def ${MTREE} -p ${MOUNT_POINT}/ -u
	TOPDIR=${TOP} CURDIR=${.CURDIR} OBJDIR=${.OBJDIR} \
	    REV=${REV} TARGDIR=${MOUNT_POINT} UTILS=${UTILS} \
	    RELEASEDIR=${RELEASEDIR} sh ${UTILS}/runlist.sh ${LISTS}
	rm ${MOUNT_POINT}/${CBIN}

clean cleandir:
	/bin/rm -f *.core ${IMAGE} ${CBIN} ${CBIN}.mk ${CBIN}.cache \
	    *.o *.lo *.c \
	    lib*.a lib*.olist ${CBIN}.map

.include <bsd.obj.mk>
.include <bsd.subdir.mk>
