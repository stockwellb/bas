#include "bas_test.h"
#include "bas_geometry.h"

/* helpers */

static void check_bas_vec2_eq(bas_vec2 actual, bas_vec2 expected) {
  CHECK_FLOAT_EQ(actual.x, expected.x);
  CHECK_FLOAT_EQ(actual.y, expected.y);
}

/* approx_eq */

static void
bas_vec2_approx_eq_returns_true_when_both_components_within_epsilon(void) {
  const bas_vec2 nudge = BAS_VEC2(WITHIN_EPSILON, WITHIN_EPSILON);
  CHECK_TRUE(bas_vec2_approx_eq(BAS_VEC2(0.0f, 0.0f), nudge, TEST_EPSILON));
}

static void bas_vec2_approx_eq_returns_false_when_x_differs_beyond_epsilon(void) {
  const bas_vec2 nudge = BAS_VEC2(BEYOND_EPSILON, 0.0f);
  CHECK_FALSE(bas_vec2_approx_eq(BAS_VEC2(0.0f, 0.0f), nudge, TEST_EPSILON));
}

static void bas_vec2_approx_eq_returns_false_when_y_differs_beyond_epsilon(void) {
  const bas_vec2 nudge = BAS_VEC2(0.0f, BEYOND_EPSILON);
  CHECK_FALSE(bas_vec2_approx_eq(BAS_VEC2(0.0f, 0.0f), nudge, TEST_EPSILON));
}

static void bas_vec2_approx_eq_respects_custom_epsilon(void) {
  const bas_vec2 nudge = BAS_VEC2(BEYOND_EPSILON, BEYOND_EPSILON);

  CHECK_FALSE(bas_vec2_approx_eq(BAS_VEC2(0.0f, 0.0f), nudge, TEST_EPSILON));
  CHECK_TRUE(bas_vec2_approx_eq(BAS_VEC2(0.0f, 0.0f), nudge, 0.1f));
}

/* add */

static void bas_vec2_add_sums_components(void) {
  check_bas_vec2_eq(bas_vec2_add(BAS_VEC2(1.0f, -2.0f), BAS_VEC2(3.0f, 4.0f)),
                BAS_VEC2(4.0f, 2.0f));
}

static void bas_vec2_add_is_commutative(void) {
  const bas_vec2 a = BAS_VEC2(1.0f, -2.0f);
  const bas_vec2 b = BAS_VEC2(3.0f, 4.0f);

  CHECK_TRUE(bas_vec2_approx_eq(bas_vec2_add(a, b), bas_vec2_add(b, a), TEST_EPSILON));
}

/* sub */

static void bas_vec2_sub_subtracts_components(void) {
  check_bas_vec2_eq(bas_vec2_sub(BAS_VEC2(5.0f, 7.0f), BAS_VEC2(2.0f, 3.0f)), BAS_VEC2(3.0f, 4.0f));
}

static void bas_vec2_sub_of_vector_with_itself_is_zero(void) {
  const bas_vec2 v = BAS_VEC2(1.0f, -2.0f);
  check_bas_vec2_eq(bas_vec2_sub(v, v), BAS_VEC2(0.0f, 0.0f));
}

/* scale */

static void bas_vec2_scale_multiplies_both_components(void) {
  check_bas_vec2_eq(bas_vec2_scale(BAS_VEC2(2.0f, 3.0f), 4.0f), BAS_VEC2(8.0f, 12.0f));
}

static void bas_vec2_scale_by_zero_yields_zero_vector(void) {
  check_bas_vec2_eq(bas_vec2_scale(BAS_VEC2(1.0f, -2.0f), 0.0f), BAS_VEC2(0.0f, 0.0f));
}

static void bas_vec2_scale_by_negative_factor_flips_direction(void) {
  check_bas_vec2_eq(bas_vec2_scale(BAS_VEC2(1.0f, 2.0f), -1.0f), BAS_VEC2(-1.0f, -2.0f));
}

/* dot */

static void bas_vec2_dot_of_mixed_sign_vectors(void) {
  CHECK_FLOAT_EQ(bas_vec2_dot(BAS_VEC2(1.0f, -2.0f), BAS_VEC2(3.0f, 4.0f)), -5.0f);
}

static void bas_vec2_dot_of_orthogonal_vectors_is_zero(void) {
  CHECK_FLOAT_EQ(bas_vec2_dot(BAS_VEC2(1.0f, 0.0f), BAS_VEC2(0.0f, 1.0f)), 0.0f);
}

/* length */

static void bas_vec2_length_sq_of_3_4_is_25(void) {
  CHECK_FLOAT_EQ(bas_vec2_length_sq(BAS_VEC2(3.0f, 4.0f)), 25.0f);
}

static void bas_vec2_length_sq_of_zero_vector_is_zero(void) {
  CHECK_FLOAT_EQ(bas_vec2_length_sq(BAS_VEC2(0.0f, 0.0f)), 0.0f);
}

static void bas_vec2_length_of_3_4_is_5(void) {
  CHECK_FLOAT_EQ(bas_vec2_length(BAS_VEC2(3.0f, 4.0f)), 5.0f);
}

static void bas_vec2_length_of_zero_vector_is_zero(void) {
  CHECK_FLOAT_EQ(bas_vec2_length(BAS_VEC2(0.0f, 0.0f)), 0.0f);
}

/* normalize */

static void bas_vec2_normalize_of_3_4_is_0_6_0_8(void) {
  check_bas_vec2_eq(bas_vec2_normalize(BAS_VEC2(3.0f, 4.0f)), BAS_VEC2(0.6f, 0.8f));
}

static void bas_vec2_normalize_of_zero_vector_returns_zero_vector(void) {
  check_bas_vec2_eq(bas_vec2_normalize(BAS_VEC2(0.0f, 0.0f)), BAS_VEC2(0.0f, 0.0f));
}

static void bas_vec2_normalized_vector_has_length_1(void) {
  CHECK_FLOAT_EQ(bas_vec2_length(bas_vec2_normalize(BAS_VEC2(3.0f, 4.0f))), 1.0f);
}

static void bas_vec2_normalize_of_unit_vector_is_unchanged(void) {
  const bas_vec2 v = BAS_VEC2(0.6f, 0.8f);
  check_bas_vec2_eq(bas_vec2_normalize(v), v);
}

/* negate */

static void bas_vec2_negate_flips_sign_of_both_components(void) {
  check_bas_vec2_eq(bas_vec2_negate(BAS_VEC2(3.0f, 4.0f)), BAS_VEC2(-3.0f, -4.0f));
}

static void bas_vec2_negating_twice_returns_original(void) {
  const bas_vec2 v = BAS_VEC2(3.0f, -4.0f);
  check_bas_vec2_eq(bas_vec2_negate(bas_vec2_negate(v)), v);
}

/* distance */

static void bas_vec2_distance_between_two_points(void) {
  CHECK_FLOAT_EQ(bas_vec2_distance(BAS_VEC2(1.0f, 2.0f), BAS_VEC2(4.0f, 6.0f)), 5.0f);
}

static void bas_vec2_distance_from_point_to_itself_is_zero(void) {
  const bas_vec2 v = BAS_VEC2(1.0f, 2.0f);
  CHECK_FLOAT_EQ(bas_vec2_distance(v, v), 0.0f);
}

static void bas_vec2_distance_is_symmetric(void) {
  const bas_vec2 a = BAS_VEC2(1.0f, 2.0f);
  const bas_vec2 b = BAS_VEC2(4.0f, 6.0f);

  CHECK_FLOAT_EQ(bas_vec2_distance(a, b), bas_vec2_distance(b, a));
}

/* distance_sq */

static void bas_vec2_distance_sq_between_two_points(void) {
  CHECK_FLOAT_EQ(bas_vec2_distance_sq(BAS_VEC2(1.0f, 2.0f), BAS_VEC2(4.0f, 6.0f)), 25.0f);
}

static void bas_vec2_distance_sq_from_point_to_itself_is_zero(void) {
  const bas_vec2 v = BAS_VEC2(1.0f, 2.0f);
  CHECK_FLOAT_EQ(bas_vec2_distance_sq(v, v), 0.0f);
}

static void bas_vec2_distance_sq_is_symmetric(void) {
  const bas_vec2 a = BAS_VEC2(1.0f, 2.0f);
  const bas_vec2 b = BAS_VEC2(4.0f, 6.0f);

  CHECK_FLOAT_EQ(bas_vec2_distance_sq(a, b), bas_vec2_distance_sq(b, a));
}

/* lerp */

static void bas_vec2_lerp_at_half_returns_midpoint(void) {
  check_bas_vec2_eq(bas_vec2_lerp(BAS_VEC2(0.0f, 0.0f), BAS_VEC2(10.0f, 10.0f), 0.5f),
                BAS_VEC2(5.0f, 5.0f));
}

static void bas_vec2_lerp_at_zero_returns_start_vector(void) {
  const bas_vec2 a = BAS_VEC2(3.0f, 3.0f);
  const bas_vec2 b = BAS_VEC2(10.0f, 10.0f);

  check_bas_vec2_eq(bas_vec2_lerp(a, b, 0.0f), a);
}

static void bas_vec2_lerp_at_one_returns_end_vector(void) {
  const bas_vec2 a = BAS_VEC2(3.0f, 3.0f);
  const bas_vec2 b = BAS_VEC2(10.0f, 10.0f);

  check_bas_vec2_eq(bas_vec2_lerp(a, b, 1.0f), b);
}

/* cross */

static void bas_vec2_cross_of_orthogonal_basis_vectors_is_1(void) {
  CHECK_FLOAT_EQ(bas_vec2_cross(BAS_VEC2(1.0f, 0.0f), BAS_VEC2(0.0f, 1.0f)), 1.0f);
}

static void bas_vec2_cross_of_collinear_vectors_is_zero(void) {
  CHECK_FLOAT_EQ(bas_vec2_cross(BAS_VEC2(2.0f, 3.0f), BAS_VEC2(4.0f, 6.0f)), 0.0f);
  CHECK_FLOAT_EQ(bas_vec2_cross(BAS_VEC2(2.0f, 3.0f), BAS_VEC2(-4.0f, -6.0f)), 0.0f);
}

int main(void) {
  /* approx_eq */
  RUN_TEST(bas_vec2_approx_eq_returns_true_when_both_components_within_epsilon);
  RUN_TEST(bas_vec2_approx_eq_returns_false_when_x_differs_beyond_epsilon);
  RUN_TEST(bas_vec2_approx_eq_returns_false_when_y_differs_beyond_epsilon);
  RUN_TEST(bas_vec2_approx_eq_respects_custom_epsilon);

  /* add */
  RUN_TEST(bas_vec2_add_sums_components);
  RUN_TEST(bas_vec2_add_is_commutative);

  /* sub */
  RUN_TEST(bas_vec2_sub_subtracts_components);
  RUN_TEST(bas_vec2_sub_of_vector_with_itself_is_zero);

  /* scale */
  RUN_TEST(bas_vec2_scale_multiplies_both_components);
  RUN_TEST(bas_vec2_scale_by_zero_yields_zero_vector);
  RUN_TEST(bas_vec2_scale_by_negative_factor_flips_direction);

  /* dot */
  RUN_TEST(bas_vec2_dot_of_mixed_sign_vectors);
  RUN_TEST(bas_vec2_dot_of_orthogonal_vectors_is_zero);

  /* length */
  RUN_TEST(bas_vec2_length_sq_of_3_4_is_25);
  RUN_TEST(bas_vec2_length_sq_of_zero_vector_is_zero);
  RUN_TEST(bas_vec2_length_of_3_4_is_5);
  RUN_TEST(bas_vec2_length_of_zero_vector_is_zero);

  /* normalize */
  RUN_TEST(bas_vec2_normalize_of_3_4_is_0_6_0_8);
  RUN_TEST(bas_vec2_normalize_of_zero_vector_returns_zero_vector);
  RUN_TEST(bas_vec2_normalized_vector_has_length_1);
  RUN_TEST(bas_vec2_normalize_of_unit_vector_is_unchanged);

  /* negate */
  RUN_TEST(bas_vec2_negate_flips_sign_of_both_components);
  RUN_TEST(bas_vec2_negating_twice_returns_original);

  /* distance */
  RUN_TEST(bas_vec2_distance_between_two_points);
  RUN_TEST(bas_vec2_distance_from_point_to_itself_is_zero);
  RUN_TEST(bas_vec2_distance_is_symmetric);

  /* distance_sq */
  RUN_TEST(bas_vec2_distance_sq_between_two_points);
  RUN_TEST(bas_vec2_distance_sq_from_point_to_itself_is_zero);
  RUN_TEST(bas_vec2_distance_sq_is_symmetric);

  /* lerp */
  RUN_TEST(bas_vec2_lerp_at_half_returns_midpoint);
  RUN_TEST(bas_vec2_lerp_at_zero_returns_start_vector);
  RUN_TEST(bas_vec2_lerp_at_one_returns_end_vector);

  /* cross */
  RUN_TEST(bas_vec2_cross_of_orthogonal_basis_vectors_is_1);
  RUN_TEST(bas_vec2_cross_of_collinear_vectors_is_zero);

  TEST_SUMMARY();

  return bas_tests_failed > 0;
}
