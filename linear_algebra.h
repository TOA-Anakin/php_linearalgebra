#ifndef LINEAR_ALGEBRA_H
#define LINEAR_ALGEBRA_H

extern "C" {
    #ifdef HAVE_CONFIG_H
    #include "config.h"
    #endif

    #include "php.h"

    PHP_FUNCTION(solve_linear_system);

    extern zend_module_entry linear_algebra_module_entry;
}

#endif /* LINEAR_ALGEBRA_H */