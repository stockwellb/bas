#include "bas_geometry.h"
#include "bas_test.h"

#include <math.h>

/* bas_scalar_approx_eq */

static void approx_eq_returns_true_for_values_within_epsilon(void) {
  CHECK_TRUE(bas_scalar_approx_eq(1.0f, 1.0f - WITHIN_EPSILON, TEST_EPSILON));
}

static void approx_eq_returns_false_for_values_beyond_epsilon(void) {
  CHECK_FALSE(bas_scalar_approx_eq(1.0f, 1.0f + BEYOND_EPSILON, TEST_EPSILON));
}

static void approx_eq_treats_equal_values_as_equal(void) {
  CHECK_TRUE(bas_scalar_approx_eq(1.0f, 1.0f, TEST_EPSILON));
}

static void approx_eq_returns_false_for_nan_values(void) {
  CHECK_FALSE(bas_scalar_approx_eq(NAN, 1.0f, TEST_EPSILON));
  CHECK_FALSE(bas_scalar_approx_eq(1.0f, NAN, TEST_EPSILON));
  CHECK_FALSE(bas_scalar_approx_eq(NAN, NAN, TEST_EPSILON));
}

/* bas_scalar_lerp */

static void lerp_returns_a_when_t_is_zero(void) {
  CHECK_FLOAT_EQ(bas_scalar_lerp(1.0f, 2.0f, 0.0f), 1.0f);
}

static void lerp_returns_b_when_t_is_one(void) {
  CHECK_FLOAT_EQ(bas_scalar_lerp(1.0f, 2.0f, 1.0f), 2.0f);
}

static void lerp_returns_midpoint_when_t_is_point_five(void) {
  CHECK_FLOAT_EQ(bas_scalar_lerp(1.0f, 2.0f, 0.5f), 1.5f);
}

/* bas_scalar_lerp_clamped */

static void lerp_clamped_returns_b_when_t_is_above_one(void) {
  CHECK_FLOAT_EQ(bas_scalar_lerp_clamped(1.0f, 2.0f, 100.0f), 2.0f);
}

static void lerp_clamped_returns_a_when_t_is_below_zero(void) {
  CHECK_FLOAT_EQ(bas_scalar_lerp_clamped(1.0f, 2.0f, -100.0f), 1.0f);
}

static void lerp_clamped_interpolates_when_t_is_in_range(void) {
  CHECK_FLOAT_EQ(bas_scalar_lerp_clamped(1.0f, 2.0f, 0.5f), 1.5f);
}

/* bas_scalar_clamp */
static void clamp_returns_min_when_value_is_below_range(void) {
  CHECK_FLOAT_EQ(bas_scalar_clamp(-1.0f, 0.0f, 1.0f), 0.0f);
}

static void clamp_returns_max_when_value_is_above_range(void) {
  CHECK_FLOAT_EQ(bas_scalar_clamp(2.0f, 0.0f, 1.0f), 1.0f);
}

static void clamp_returns_value_when_value_is_in_range(void) {
  CHECK_FLOAT_EQ(bas_scalar_clamp(0.5f, 0.0f, 1.0f), 0.5f);
}

/* bas_scalar_clamp01 */

static void clamp01_returns_zero_when_value_is_below_zero(void) {
  CHECK_FLOAT_EQ(bas_scalar_clamp01(-1.0f), 0.0f);
}

static void clamp01_returns_one_when_value_is_above_one(void) {
  CHECK_FLOAT_EQ(bas_scalar_clamp01(2.0f), 1.0f);
}

static void clamp01_returns_value_when_value_is_in_range(void) {
  CHECK_FLOAT_EQ(bas_scalar_clamp01(0.5f), 0.5f);
}

/* bas_scalar_inverse_lerp */

static void inverse_lerp_returns_zero_when_value_equals_a(void) {
  CHECK_FLOAT_EQ(bas_scalar_inverse_lerp(1.0f, 2.0f, 1.0f), 0.0f);
}

static void inverse_lerp_returns_one_when_value_equals_b(void) {
  CHECK_FLOAT_EQ(bas_scalar_inverse_lerp(1.0f, 2.0f, 2.0f), 1.0f);
}

static void inverse_lerp_returns_half_for_midpoint(void) {
  CHECK_FLOAT_EQ(bas_scalar_inverse_lerp(1.0f, 2.0f, 1.5f), 0.5f);
}

static void inverse_lerp_returns_zero_when_a_and_b_are_equal(void) {
  CHECK_FLOAT_EQ(bas_scalar_inverse_lerp(1.0f, 1.0f, 1.0f), 0.0f);
}

/* bas_scalar_wrap */

static void wrap_wraps_positive_values(void) {
  CHECK_FLOAT_EQ(bas_scalar_wrap(7.0f, 5.0f), 2.0f);
}

static void wrap_wraps_negative_values(void) {
  CHECK_FLOAT_EQ(bas_scalar_wrap(-1.0f, 5.0f), 4.0f);
}

static void wrap_returns_zero_for_multiples_of_length(void) {
  CHECK_FLOAT_EQ(bas_scalar_wrap(10.0f, 5.0f), 0.0f);
}

static void wrap_returns_value_when_already_in_range(void) {
  CHECK_FLOAT_EQ(bas_scalar_wrap(2.0f, 5.0f), 2.0f);
}

/* bas_scalar_signf */

static void signf_returns_one_for_positive_values(void) {
  CHECK_FLOAT_EQ(bas_scalar_signf(5.0f), 1.0f);
}

static void signf_returns_negative_one_for_negative_values(void) {
  CHECK_FLOAT_EQ(bas_scalar_signf(-5.0f), -1.0f);
}

static void signf_returns_zero_for_zero(void) {
  CHECK_FLOAT_EQ(bas_scalar_signf(0.0f), 0.0f);
}

static void signf_returns_zero_for_negative_zero(void) {
  CHECK_FLOAT_EQ(bas_scalar_signf(-0.0f), 0.0f);
}

static void signf_returns_nan_for_nan(void) { CHECK_TRUE(isnan(bas_scalar_signf(NAN))); }

/* bas_scalar_smooth_step */

static void smooth_step_returns_zero_below_edge0(void) {
  CHECK_FLOAT_EQ(bas_scalar_smooth_step(0.0f, 1.0f, -0.5f), 0.0f);
}

static void smooth_step_returns_zero_at_edge0(void) {
  CHECK_FLOAT_EQ(bas_scalar_smooth_step(0.0f, 1.0f, 0.0f), 0.0f);
}

static void smooth_step_returns_one_above_edge1(void) {
  CHECK_FLOAT_EQ(bas_scalar_smooth_step(0.0f, 1.0f, 1.5f), 1.0f);
}

static void smooth_step_returns_one_at_edge1(void) {
  CHECK_FLOAT_EQ(bas_scalar_smooth_step(0.0f, 1.0f, 1.0f), 1.0f);
}

static void smooth_step_returns_half_at_midpoint(void) {
  CHECK_FLOAT_EQ(bas_scalar_smooth_step(0.0f, 1.0f, 0.5f), 0.5f);
}

static void smooth_step_returns_zero_when_edges_are_equal(void) {
  CHECK_FLOAT_EQ(bas_scalar_smooth_step(1.0f, 1.0f, 0.5f), 0.0f);
}

int main(void) {
  /* approx equal */
  RUN_TEST(approx_eq_returns_true_for_values_within_epsilon);
  RUN_TEST(approx_eq_returns_false_for_values_beyond_epsilon);
  RUN_TEST(approx_eq_treats_equal_values_as_equal);
  RUN_TEST(approx_eq_returns_false_for_nan_values);

  /* bas_scalar_lerp */
  RUN_TEST(lerp_returns_a_when_t_is_zero);
  RUN_TEST(lerp_returns_b_when_t_is_one);
  RUN_TEST(lerp_returns_midpoint_when_t_is_point_five);

  /* bas_scalar_lerp clamped */
  RUN_TEST(lerp_clamped_returns_b_when_t_is_above_one);
  RUN_TEST(lerp_clamped_returns_a_when_t_is_below_zero);
  RUN_TEST(lerp_clamped_interpolates_when_t_is_in_range);

  /* bas_scalar_clamp */
  RUN_TEST(clamp_returns_min_when_value_is_below_range);
  RUN_TEST(clamp_returns_max_when_value_is_above_range);
  RUN_TEST(clamp_returns_value_when_value_is_in_range);

  /* bas_scalar_clamp01 */
  RUN_TEST(clamp01_returns_zero_when_value_is_below_zero);
  RUN_TEST(clamp01_returns_one_when_value_is_above_one);
  RUN_TEST(clamp01_returns_value_when_value_is_in_range);

  /* inverse bas_scalar_lerp */
  RUN_TEST(inverse_lerp_returns_zero_when_value_equals_a);
  RUN_TEST(inverse_lerp_returns_one_when_value_equals_b);
  RUN_TEST(inverse_lerp_returns_half_for_midpoint);
  RUN_TEST(inverse_lerp_returns_zero_when_a_and_b_are_equal);

  /* bas_scalar_wrap */
  RUN_TEST(wrap_wraps_positive_values);
  RUN_TEST(wrap_wraps_negative_values);
  RUN_TEST(wrap_returns_zero_for_multiples_of_length);
  RUN_TEST(wrap_returns_value_when_already_in_range);

  /* bas_scalar_signf */
  RUN_TEST(signf_returns_one_for_positive_values);
  RUN_TEST(signf_returns_negative_one_for_negative_values);
  RUN_TEST(signf_returns_zero_for_zero);
  RUN_TEST(signf_returns_zero_for_negative_zero);
  RUN_TEST(signf_returns_nan_for_nan);

  /* smooth step */
  RUN_TEST(smooth_step_returns_zero_below_edge0);
  RUN_TEST(smooth_step_returns_zero_at_edge0);
  RUN_TEST(smooth_step_returns_one_above_edge1);
  RUN_TEST(smooth_step_returns_one_at_edge1);
  RUN_TEST(smooth_step_returns_half_at_midpoint);
  RUN_TEST(smooth_step_returns_zero_when_edges_are_equal);

  TEST_SUMMARY();

  return bas_tests_failed > 0;
}
