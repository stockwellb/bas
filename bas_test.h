/* bas_test.h - v0.1 - minimal unit-test harness - (bas)

   A single-file unit-testing microlibrary.

   Do this:

       #define BAS_TEST_IMPLEMENTATION

   before you include this file in *one* C or C++ file to create the
   implementation. That file should look like this:

       #define BAS_TEST_IMPLEMENTATION
       #include "bas_test.h"

   All other files should just #include "bas_test.h" without the #define.


   DOCUMENTATION

   Write test functions of type `void(void)`, make assertions with the CHECK_*
   macros, run them with RUN_TEST, and print a tally with TEST_SUMMARY:

       static void one_plus_one_is_two(void) {
         CHECK_EQ_INT(1 + 1, 2);
       }

       int main(void) {
         RUN_TEST(one_plus_one_is_two);
         TEST_SUMMARY();
         return bas_tests_failed > 0;   // non-zero exit on failure
       }


   COMPILE-TIME OPTIONS

   #define BAS_TESTDEF <specifier>

       Controls linkage of the harness functions. Defaults to `extern` for
       declarations and nothing for definitions.


   LICENSE

   MIT License. See the LICENSE file distributed with this project.
*/

#ifndef BAS_INCLUDE_BAS_TEST_H
#define BAS_INCLUDE_BAS_TEST_H

#include <stdbool.h>
#include <stddef.h>

#ifndef BAS_TESTDEF
#ifdef BAS_TEST_STATIC
#define BAS_TESTDEF static
#else
#define BAS_TESTDEF extern
#endif
#endif

/* Running tallies, updated by every CHECK_* assertion. */
extern unsigned int bas_tests_run;
extern unsigned int bas_tests_failed;

BAS_TESTDEF void bas_test_check(bool passed, const char *expr, const char *file,
                                int line);

BAS_TESTDEF void bas_test_check_eq_int(int actual, int expected,
                                       const char *actual_expr,
                                       const char *expected_expr,
                                       const char *file, int line);

BAS_TESTDEF void bas_test_check_ne_int(int actual, int expected,
                                       const char *actual_expr,
                                       const char *expected_expr,
                                       const char *file, int line);

BAS_TESTDEF void bas_test_check_eq_float(float actual, float expected,
                                         float epsilon, const char *actual_expr,
                                         const char *expected_expr,
                                         const char *file, int line);

BAS_TESTDEF void bas_test_check_null(const void *ptr, const char *ptr_expr,
                                     const char *file, int line);

BAS_TESTDEF void bas_test_check_not_null(const void *ptr, const char *ptr_expr,
                                         const char *file, int line);

BAS_TESTDEF void bas_test_run(void (*test_fn)(void), const char *name);
BAS_TESTDEF void bas_test_summary(void);

#define TEST_EPSILON 1e-5f
#define WITHIN_EPSILON (TEST_EPSILON / 2.0f)
#define BEYOND_EPSILON (TEST_EPSILON * 2.0f)

#define RUN_TEST(fn) bas_test_run((fn), #fn)

#define CHECK(cond) bas_test_check(!!(cond), #cond, __FILE__, __LINE__)

#define CHECK_TRUE(cond)                                                       \
  bas_test_check(!!(cond), #cond " is true", __FILE__, __LINE__)

#define CHECK_FALSE(cond)                                                      \
  bas_test_check(!(cond), #cond " is false", __FILE__, __LINE__)

#define CHECK_EQ_INT(actual, expected)                                         \
  bas_test_check_eq_int((actual), (expected), #actual, #expected, __FILE__,    \
                        __LINE__)

#define CHECK_NE_INT(actual, expected)                                         \
  bas_test_check_ne_int((actual), (expected), #actual, #expected, __FILE__,    \
                        __LINE__)

#define CHECK_FLOAT_EQ(actual, expected)                                       \
  bas_test_check_eq_float((actual), (expected), TEST_EPSILON, #actual,         \
                          #expected, __FILE__, __LINE__)

#define CHECK_FLOAT_EQ_EPS(actual, expected, eps)                              \
  bas_test_check_eq_float((actual), (expected), eps, #actual, #expected,       \
                          __FILE__, __LINE__)

#define CHECK_NULL(ptr) bas_test_check_null((ptr), #ptr, __FILE__, __LINE__)

#define CHECK_NOT_NULL(ptr)                                                    \
  bas_test_check_not_null((ptr), #ptr, __FILE__, __LINE__)

#define TEST_SUMMARY() bas_test_summary()

#endif /* BAS_INCLUDE_BAS_TEST_H */


/* ------------------------------------------------------------------------- */
/* IMPLEMENTATION                                                            */
/* ------------------------------------------------------------------------- */

#ifdef BAS_TEST_IMPLEMENTATION

#include <math.h>
#include <stdio.h>

unsigned int bas_tests_run = 0;
unsigned int bas_tests_failed = 0;

static void bas_test__pass(const char *expr) { printf("  [PASS] %s\n", expr); }

static void bas_test__fail(const char *expr, const char *file, int line) {
  bas_tests_failed++;
  printf("  [FAIL] %s\n", expr);
  printf("         at %s:%d\n", file, line);
}

BAS_TESTDEF void bas_test_check(bool passed, const char *expr, const char *file,
                                int line) {
  bas_tests_run++;

  if (passed) {
    bas_test__pass(expr);
  } else {
    bas_test__fail(expr, file, line);
  }
}

BAS_TESTDEF void bas_test_check_eq_int(int actual, int expected,
                                       const char *actual_expr,
                                       const char *expected_expr,
                                       const char *file, int line) {
  bas_tests_run++;

  if (actual == expected) {
    printf("  [PASS] %s == %s\n", actual_expr, expected_expr);
    return;
  }

  bas_test__fail("integer equality failed", file, line);
  printf("         actual:   %s = %d\n", actual_expr, actual);
  printf("         expected: %s = %d\n", expected_expr, expected);
}

BAS_TESTDEF void bas_test_check_ne_int(int actual, int expected,
                                       const char *actual_expr,
                                       const char *expected_expr,
                                       const char *file, int line) {
  bas_tests_run++;

  if (actual != expected) {
    printf("  [PASS] %s != %s\n", actual_expr, expected_expr);
    return;
  }

  bas_test__fail("integer inequality failed", file, line);
  printf("         actual:   %s = %d\n", actual_expr, actual);
  printf("         expected not to equal: %s = %d\n", expected_expr, expected);
}

BAS_TESTDEF void bas_test_check_eq_float(float actual, float expected,
                                         float epsilon, const char *actual_expr,
                                         const char *expected_expr,
                                         const char *file, int line) {
  float diff = fabsf(actual - expected);

  bas_tests_run++;

  if (diff < epsilon) {
    printf("  [PASS] %s ~= %s\n", actual_expr, expected_expr);
    return;
  }

  bas_test__fail("float equality failed", file, line);
  printf("         actual:   %s = %.9g\n", actual_expr, actual);
  printf("         expected: %s = %.9g\n", expected_expr, expected);
  printf("         diff:     %.9g\n", diff);
  printf("         epsilon:  %.9g\n", epsilon);
}

BAS_TESTDEF void bas_test_check_null(const void *ptr, const char *ptr_expr,
                                     const char *file, int line) {
  bas_tests_run++;

  if (ptr == NULL) {
    printf("  [PASS] %s == NULL\n", ptr_expr);
    return;
  }

  bas_test__fail("null check failed", file, line);
  printf("         actual: %s = %p\n", ptr_expr, ptr);
}

BAS_TESTDEF void bas_test_check_not_null(const void *ptr, const char *ptr_expr,
                                         const char *file, int line) {
  bas_tests_run++;

  if (ptr != NULL) {
    printf("  [PASS] %s != NULL\n", ptr_expr);
    return;
  }

  bas_test__fail("not-null check failed", file, line);
  printf("         actual: %s = NULL\n", ptr_expr);
}

BAS_TESTDEF void bas_test_run(void (*test_fn)(void), const char *name) {
  printf("\n=== %s ===\n", name);
  test_fn();
}

BAS_TESTDEF void bas_test_summary(void) {
  unsigned int passed = bas_tests_run - bas_tests_failed;

  printf("\n=== TEST SUMMARY ===\n");
  printf("Run:    %u\n", bas_tests_run);
  printf("Passed: %u\n", passed);
  printf("Failed: %u\n", bas_tests_failed);
  printf("Result: %s\n", bas_tests_failed == 0 ? "PASS" : "FAIL");
}

#endif /* BAS_TEST_IMPLEMENTATION */
