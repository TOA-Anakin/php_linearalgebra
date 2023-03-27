#include <iostream>
#include <sstream>
#include <string>
#include <php.h>
#include "zend_API.h"
#include "Zend/zend_string.h"
#include "zend_exceptions.h"
#include "php_linearalgebra.h"
#include <eigen3/Eigen/Dense>

// standard c++ namespace
using namespace std;
using namespace Eigen;

ZEND_BEGIN_ARG_INFO_EX(arginfo_solveSLE, 0, 0, 4)
    ZEND_ARG_INFO(0, A)
    ZEND_ARG_INFO(0, b)
    ZEND_ARG_INFO(0, method)
    ZEND_ARG_INFO(0, calcDet)
ZEND_END_ARG_INFO()

zend_function_entry linearalgebra_php_functions[] = {
    PHP_FE(solveSLE_php, arginfo_solveSLE)
    PHP_FE_END
};

zend_module_entry linearalgebra_php_module_entry = {
    STANDARD_MODULE_HEADER,
    PHP_LINEARALGEBRA_EXTNAME,
    linearalgebra_php_functions,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    PHP_LINEARALGEBRA_VERSION,
    STANDARD_MODULE_PROPERTIES
};

ZEND_GET_MODULE(linearalgebra_php)

PHP_FUNCTION(solveSLE_php) {
    zval *A_zval, *b_zval, *method_zval, *calcDet_zval;

    if (zend_parse_parameters(ZEND_NUM_ARGS(), "aazb", &A_zval, &b_zval, &method_zval, &calcDet_zval) == FAILURE) {
        RETURN_NULL();
    }

    int n = zend_hash_num_elements(Z_ARRVAL_P(A_zval));

    convert_to_string(method_zval);
    string method = Z_STRVAL_P(method_zval);

    double detA = 0.0;

    int i = 0;
    int j = 0;
    int k = 0;

    Eigen::MatrixXd A_tmp(n, n);
    if (calcDet_zval) {
        zval *row;
        ZEND_HASH_FOREACH_VAL(Z_ARRVAL_P(A_zval), row) {
            if (Z_TYPE_P(row) == IS_ARRAY) {
                zval* A_val_zval;
                ZEND_HASH_FOREACH_VAL(Z_ARRVAL_P(row), A_val_zval) {
                    zval A_val_zval_dup;
                    ZVAL_COPY_VALUE(&A_val_zval_dup, A_val_zval);
                    convert_to_double(&A_val_zval_dup);
                    double A_val = Z_DVAL(A_val_zval_dup);
                    A_tmp(i, j) = A_val;
                    j++;
                } ZEND_HASH_FOREACH_END();
            }
            i++;
            j = 0;
        } ZEND_HASH_FOREACH_END();

        PartialPivLU<MatrixXd> lu(A_tmp);
        detA = lu.determinant();

        if (detA == 0) {
            RETURN_NULL();
        }
    }

    if (method == "default") {
        if (n <= 20) {
            method = "gaussianElimination";
        } else {
            method = "colPivHouseholderQr";
        }
    }

    i = 0;
    j = 0;

    if (method == "gaussianElimination") {
        double A[n][n+1];
        double x[n];

        zval *row;
        ZEND_HASH_FOREACH_VAL(Z_ARRVAL_P(A_zval), row) {
            if (Z_TYPE_P(row) == IS_ARRAY) {
                zval* A_val_zval;
                ZEND_HASH_FOREACH_VAL(Z_ARRVAL_P(row), A_val_zval) {
                    zval A_val_zval_dup;
                    ZVAL_COPY_VALUE(&A_val_zval_dup, A_val_zval);
                    convert_to_double(&A_val_zval_dup);
                    double A_val = Z_DVAL(A_val_zval_dup);
                    A[i][j] = A_val;
                    j++;
                } ZEND_HASH_FOREACH_END();
            }
            i++;
            j = 0;
        } ZEND_HASH_FOREACH_END();

        zval* b_val_zval;
        ZEND_HASH_FOREACH_VAL(Z_ARRVAL_P(b_zval), b_val_zval) {
            zval b_val_zval_dup;
            ZVAL_COPY_VALUE(&b_val_zval_dup, b_val_zval);
            convert_to_double(&b_val_zval_dup);
            double b_val = Z_DVAL(b_val_zval_dup);
            A[k][n] = b_val;
            k++;
        } ZEND_HASH_FOREACH_END();

        for (i = 0; i < n; i++) {                   
            for (j = i + 1;j < n; j++) {
                if (abs(A[i][i]) < abs(A[j][i])) {
                    for (k = 0; k < n + 1; k++) {
                        A[i][k] = A[i][k] + A[j][k];
                        A[j][k] = A[i][k] - A[j][k];
                        A[i][k] = A[i][k] - A[j][k];
                    }
                }
            }
        }

        for (i = 0; i < n - 1; i++) {
            for (j = i + 1; j < n; j++) {
                double f = A[j][i] / A[i][i];
                for (k = 0; k < n + 1; k++) {
                    A[j][k] = A[j][k] - f * A[i][k];
                }
            }
        }

        for (i = n - 1; i >= 0; i--) {
            x[i] = A[i][n]; 
            for (j = i + 1; j < n; j++) {
                if (i != j) {
                    x[i] = x[i] - A[i][j] * x[j];
                }          
            }
            x[i] = x[i] / A[i][i];
        }

        zval solution_zval;
        array_init_size(&solution_zval, n);

        for (int i = 0; i < n; i++) {
            add_index_double(&solution_zval, i, x[i]);
        }

        RETURN_ZVAL(&solution_zval, 0, 1);
    } else if (method == "colPivHouseholderQr") {
        Eigen::MatrixXd A(n, n);
        Eigen::VectorXd b(n);
        Eigen::VectorXd x(n);

        if (calcDet_zval) {
            A = A_tmp;
        } else {
            zval *row;
            ZEND_HASH_FOREACH_VAL(Z_ARRVAL_P(A_zval), row) {
                if (Z_TYPE_P(row) == IS_ARRAY) {
                    zval* A_val_zval;
                    ZEND_HASH_FOREACH_VAL(Z_ARRVAL_P(row), A_val_zval) {
                        zval A_val_zval_dup;
                        ZVAL_COPY_VALUE(&A_val_zval_dup, A_val_zval);
                        convert_to_double(&A_val_zval_dup);
                        double A_val = Z_DVAL(A_val_zval_dup);
                        A(i, j) = A_val;
                        j++;
                    } ZEND_HASH_FOREACH_END();
                }
                i++;
                j = 0;
            } ZEND_HASH_FOREACH_END();
        }

        zval* b_val_zval;
        ZEND_HASH_FOREACH_VAL(Z_ARRVAL_P(b_zval), b_val_zval) {
            zval b_val_zval_dup;
            ZVAL_COPY_VALUE(&b_val_zval_dup, b_val_zval);
            convert_to_double(&b_val_zval_dup);
            double b_val = Z_DVAL(b_val_zval_dup);
            b[k] = b_val;
            k++;
        } ZEND_HASH_FOREACH_END();

        x = A.colPivHouseholderQr().solve(b);

        zval solution_zval;
        array_init_size(&solution_zval, n);

        for (int i = 0; i < n; i++) {
            add_index_double(&solution_zval, i, x[i]);
        }

        RETURN_ZVAL(&solution_zval, 0, 1);
    }
}