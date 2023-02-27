#include "linear_algebra.h"

#include <Eigen/Dense>

using namespace Eigen;

PHP_FUNCTION(solve_linear_system)
{
    zval* a_zval;
    zval* b_zval;
    double* a_data;
    double* b_data;
    int n;

    if (zend_parse_parameters(ZEND_NUM_ARGS(), "aa", &a_zval, &b_zval) == FAILURE) {
        RETURN_NULL();
    }

    n = zend_hash_num_elements(Z_ARRVAL_P(a_zval));
    if (n != zend_hash_num_elements(Z_ARRVAL_P(b_zval))) {
        zend_throw_exception(NULL, "Matrix and vector dimensions don't match", 0);
        RETURN_NULL();
    }

    a_data = (double*)malloc(n*n*sizeof(double));
    b_data = (double*)malloc(n*sizeof(double));

    zval* a_entry;
    zval* b_entry;
    zend_ulong index;

    ZEND_HASH_FOREACH_NUM_KEY_VAL(Z_ARRVAL_P(a_zval), index, a_entry) {
        if (Z_TYPE_P(a_entry) != IS_ARRAY || zend_hash_num_elements(Z_ARRVAL_P(a_entry)) != n) {
            zend_throw_exception(NULL, "Invalid matrix", 0);
            free(a_data);
            free(b_data);
            RETURN_NULL();
        }

        zval* val;
        zend_ulong inner_index;

        ZEND_HASH_FOREACH_NUM_KEY_VAL(Z_ARRVAL_P(a_entry), inner_index, val) {
            if (Z_TYPE_P(val) != IS_DOUBLE) {
                zend_throw_exception(NULL, "Matrix must be a double array", 0);
                free(a_data);
                free(b_data);
                RETURN_NULL();
            }

            a_data[index*n+inner_index] = Z_DVAL_P(val);
        } ZEND_HASH_FOREACH_END();
    } ZEND_HASH_FOREACH_END();

    ZEND_HASH_FOREACH_NUM_KEY_VAL(Z_ARRVAL_P(b_zval), index, b_entry) {
        if (Z_TYPE_P(b_entry) != IS_DOUBLE) {
            zend_throw_exception(NULL, "Vector must be a double array", 0);
            free(a_data);
            free(b_data);
            RETURN_NULL();
        }

        b_data[index] = Z_DVAL_P(b_entry);
    } ZEND_HASH_FOREACH_END();

    Map<MatrixXd> A(a_data, n, n);
    Map<VectorXd> b(b_data, n);
    VectorXd x = A.colPivHouseholderQr().solve(b);

    array_init(return_value);
    for (int i = 0; i < n; i++) {
        add_index_double(return_value, i, x(i));
    }

    free(a_data);
    free(b_data);
}

zend_module_entry linear_algebra_module_entry = {
    STANDARD_MODULE_HEADER,
    "linear_algebra",
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    "0.1",
    STANDARD_MODULE_PROPERTIES
};

#ifdef COMPILE_DL_LINEAR_ALGEBRA
extern "C" {
    ZEND_GET_MODULE(linear_algebra)
}
#endif