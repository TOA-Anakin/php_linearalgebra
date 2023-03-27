PHP_ARG_ENABLE(linearalgebra, Whether to enable the LinearAlgebraPHP extension, [ --enable-linearalgebra-php Enable LinearAlgebraPHP])

if test "$PHP_LINEARALGEBRA" = "yes"; then
    PHP_REQUIRE_CXX() # support for the c++
    PHP_NEW_EXTENSION([linearalgebra], [php_linearalgebra.cpp], $ext_shared,, [-Wall], true) # setup our extension
fi
