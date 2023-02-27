PHP_ARG_WITH(linear_algebra, whether to enable the linear_algebra extension, [yes])
if test $PHP_LINEAR_ALGEBRA != "no"; then
    PHP_SUBST(LINEAR_ALGEBRA_SHARED_LIBADD)
    PHP_ADD_LIBRARY(stdc++, 1, LINEAR_ALGEBRA_SHARED_LIBADD)
    PHP_NEW_EXTENSION(linear_algebra, linear_algebra.cpp, $ext_shared)
fi
