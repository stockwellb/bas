#include "bas_geometry.h"
#include "bas_test.h"

/* helpers */

static void check_bas_vec2_eq(bas_vec2 actual, bas_vec2 expected) {
  CHECK_FLOAT_EQ(actual.x, expected.x);
  CHECK_FLOAT_EQ(actual.y, expected.y);
}

static void check_bas_aabb_eq(bas_aabb actual, bas_aabb expected) {
  check_bas_vec2_eq(actual.min, expected.min);
  check_bas_vec2_eq(actual.max, expected.max);
}

/* init */

static void bas_aabb_init_sets_min_and_max(void) {
  bas_aabb aabb;

  CHECK_EQ_INT(bas_aabb_init(&aabb, BAS_VEC2(1.0f, 2.0f), BAS_VEC2(3.0f, 4.0f)), BAS_AABB_OK);

  check_bas_aabb_eq(aabb, (bas_aabb){BAS_VEC2(1.0f, 2.0f), BAS_VEC2(3.0f, 4.0f)});
}

static void bas_aabb_init_returns_invalid_bounds_when_both_axes_are_inverted(void) {
  bas_aabb aabb;

  CHECK_EQ_INT(bas_aabb_init(&aabb, BAS_VEC2(1.0f, 1.0f), BAS_VEC2(0.0f, 0.0f)),
               BAS_AABB_INVALID_BOUNDS);
}

static void bas_aabb_init_returns_invalid_bounds_when_only_x_is_inverted(void) {
  bas_aabb aabb;

  CHECK_EQ_INT(bas_aabb_init(&aabb, BAS_VEC2(5.0f, 0.0f), BAS_VEC2(0.0f, 1.0f)),
               BAS_AABB_INVALID_BOUNDS);
}

static void bas_aabb_init_returns_invalid_bounds_when_only_y_is_inverted(void) {
  bas_aabb aabb;

  CHECK_EQ_INT(bas_aabb_init(&aabb, BAS_VEC2(0.0f, 5.0f), BAS_VEC2(1.0f, 0.0f)),
               BAS_AABB_INVALID_BOUNDS);
}

static void bas_aabb_init_allows_degenerate_box(void) {
  bas_aabb aabb;

  CHECK_EQ_INT(bas_aabb_init(&aabb, BAS_VEC2(1.0f, 1.0f), BAS_VEC2(1.0f, 1.0f)), BAS_AABB_OK);

  check_bas_aabb_eq(aabb, (bas_aabb){BAS_VEC2(1.0f, 1.0f), BAS_VEC2(1.0f, 1.0f)});
}

/* from_center_and_size */

static void bas_aabb_from_center_and_size_sets_min_and_max(void) {
  bas_aabb aabb;

  CHECK_EQ_INT(
      bas_aabb_from_center_and_size(&aabb, BAS_VEC2(0.0f, 0.0f), BAS_VEC2(2.0f, 2.0f)),
      BAS_AABB_OK);

  check_bas_aabb_eq(aabb, (bas_aabb){BAS_VEC2(-1.0f, -1.0f), BAS_VEC2(1.0f, 1.0f)});
}

static void bas_aabb_from_center_and_size_offsets_from_non_zero_center(void) {
  bas_aabb aabb;

  CHECK_EQ_INT(
      bas_aabb_from_center_and_size(&aabb, BAS_VEC2(10.0f, 20.0f), BAS_VEC2(4.0f, 6.0f)),
      BAS_AABB_OK);

  check_bas_aabb_eq(aabb, (bas_aabb){BAS_VEC2(8.0f, 17.0f), BAS_VEC2(12.0f, 23.0f)});
}

static void
bas_aabb_from_center_and_size_returns_invalid_size_when_both_axes_are_negative(
    void) {
  bas_aabb aabb;

  CHECK_EQ_INT(
      bas_aabb_from_center_and_size(&aabb, BAS_VEC2(0.0f, 0.0f), BAS_VEC2(-1.0f, -1.0f)),
      BAS_AABB_INVALID_SIZE);
}

static void
bas_aabb_from_center_and_size_returns_invalid_size_when_only_width_is_negative(
    void) {
  bas_aabb aabb;

  CHECK_EQ_INT(
      bas_aabb_from_center_and_size(&aabb, BAS_VEC2(0.0f, 0.0f), BAS_VEC2(-1.0f, 2.0f)),
      BAS_AABB_INVALID_SIZE);
}

static void
bas_aabb_from_center_and_size_returns_invalid_size_when_only_height_is_negative(
    void) {
  bas_aabb aabb;

  CHECK_EQ_INT(
      bas_aabb_from_center_and_size(&aabb, BAS_VEC2(0.0f, 0.0f), BAS_VEC2(2.0f, -1.0f)),
      BAS_AABB_INVALID_SIZE);
}

static void bas_aabb_from_center_and_size_allows_zero_size(void) {
  bas_aabb aabb;

  CHECK_EQ_INT(
      bas_aabb_from_center_and_size(&aabb, BAS_VEC2(3.0f, 4.0f), BAS_VEC2(0.0f, 0.0f)),
      BAS_AABB_OK);

  check_bas_aabb_eq(aabb, (bas_aabb){BAS_VEC2(3.0f, 4.0f), BAS_VEC2(3.0f, 4.0f)});
}

/* from_points */

static void bas_aabb_from_points_takes_each_axis_extreme_independently(void) {
  bas_aabb aabb;
  const bas_vec2 points[] = {
      BAS_VEC2(-5.0f, 3.0f),
      BAS_VEC2(2.0f, -4.0f),
      BAS_VEC2(1.0f, 10.0f),
  };

  CHECK_EQ_INT(bas_aabb_from_points(&aabb, points, 3), BAS_AABB_OK);

  check_bas_aabb_eq(aabb, (bas_aabb){BAS_VEC2(-5.0f, -4.0f), BAS_VEC2(2.0f, 10.0f)});
}

static void bas_aabb_from_points_returns_empty_points_when_count_is_zero(void) {
  bas_aabb aabb;

  CHECK_EQ_INT(bas_aabb_from_points(&aabb, NULL, 0), BAS_AABB_EMPTY_POINTS);
}

static void bas_aabb_from_points_with_single_point_yields_degenerate_box(void) {
  bas_aabb aabb;
  const bas_vec2 points[] = {
      BAS_VEC2(3.0f, 4.0f),
  };

  CHECK_EQ_INT(bas_aabb_from_points(&aabb, points, 1), BAS_AABB_OK);

  check_bas_aabb_eq(aabb, (bas_aabb){BAS_VEC2(3.0f, 4.0f), BAS_VEC2(3.0f, 4.0f)});
}

int main(void) {
  /* init */
  RUN_TEST(bas_aabb_init_sets_min_and_max);
  RUN_TEST(bas_aabb_init_returns_invalid_bounds_when_both_axes_are_inverted);
  RUN_TEST(bas_aabb_init_returns_invalid_bounds_when_only_x_is_inverted);
  RUN_TEST(bas_aabb_init_returns_invalid_bounds_when_only_y_is_inverted);
  RUN_TEST(bas_aabb_init_allows_degenerate_box);

  /* from center and size */
  RUN_TEST(bas_aabb_from_center_and_size_sets_min_and_max);
  RUN_TEST(bas_aabb_from_center_and_size_offsets_from_non_zero_center);
  RUN_TEST(
      bas_aabb_from_center_and_size_returns_invalid_size_when_both_axes_are_negative);
  RUN_TEST(
      bas_aabb_from_center_and_size_returns_invalid_size_when_only_width_is_negative);
  RUN_TEST(
      bas_aabb_from_center_and_size_returns_invalid_size_when_only_height_is_negative);
  RUN_TEST(bas_aabb_from_center_and_size_allows_zero_size);

  /* from points */
  RUN_TEST(bas_aabb_from_points_takes_each_axis_extreme_independently);
  RUN_TEST(bas_aabb_from_points_returns_empty_points_when_count_is_zero);
  RUN_TEST(bas_aabb_from_points_with_single_point_yields_degenerate_box);

  TEST_SUMMARY();

  return bas_tests_failed > 0;
}
