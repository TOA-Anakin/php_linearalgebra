PHP_ARG_WITH(linear_solver, whether to enable the linear_solver extension, [yes])
if test $PHP_LINEAR_SOLVER != "no"; then
    PHP_SUBST(LINEAR_SOLVER_SHARED_LIBADD)
    PHP_ADD_LIBRARY(stdc++, 1, LINEAR_SOLVER_SHARED_LIBADD)
    PHP_NEW_EXTENSION(linear_solver, linear_solver.cpp, $ext_shared)
fi
