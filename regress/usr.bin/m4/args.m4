dnl $OpenBSD$
dnl Expanding all arguments
define(`A', `first form: $@, second form $*')dnl
define(`B', `C')dnl
A(1,2,`B')
dnl indirection means macro can get called with argc == 2 !
indir(`A',1,2,`B')
indir(`A')
