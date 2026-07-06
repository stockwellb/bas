/* bas_geometry.h - v0.1 - scalar & 2D vector math - (bas)

   A single-file library of small scalar-float and 2D-vector helpers.

   Do this:

       #define BAS_GEOMETRY_IMPLEMENTATION

   before you include this file in *one* C or C++ file to create the
   implementation. That file should look like this:

       #include ...
       #include ...
       #define BAS_GEOMETRY_IMPLEMENTATION
       #include "bas_geometry.h"

   All other files should just #include "bas_geometry.h" without the #define.


   DOCUMENTATION

   Two groups of helpers share this file:

     - bas_scalar_* : float utilities (clamp, lerp, wrap, angles, ...).
     - bas_vec2_*   : 2D float-vector math, built on the scalar helpers.

   The vector type is a plain struct of two floats; construct one with the
   BAS_VEC2 macro and pass/return it by value:

       bas_vec2 a = BAS_VEC2(1.0f, 2.0f);
       bas_vec2 b = BAS_VEC2_ZERO;
       bas_vec2 c = bas_vec2_add(a, b);
       float    d = bas_vec2_length(c);
       float    e = bas_scalar_clamp01(d);


   COMPILE-TIME OPTIONS

   #define BAS_GEODEF <specifier>

       Controls linkage/inlining of every function. Defaults to `extern` for
       declarations and nothing for definitions. Define it (before the first
       include) to e.g. `static inline` to get a header-only, internal-linkage
       build:

           #define BAS_GEODEF static inline
           #define BAS_GEOMETRY_IMPLEMENTATION
           #include "bas_geometry.h"


   LICENSE

   MIT License. See the LICENSE file distributed with this project.
*/

#ifndef BAS_INCLUDE_BAS_GEOMETRY_H
#define BAS_INCLUDE_BAS_GEOMETRY_H

#include <stdbool.h>
#include <stddef.h>

#ifndef BAS_GEODEF
#ifdef BAS_GEOMETRY_STATIC
#define BAS_GEODEF static
#else
#define BAS_GEODEF extern
#endif
#endif

/* ========================================================================= */
/* Scalar                                                                    */
/* ========================================================================= */

#define BAS_SCALAR_PI 3.14159265358979323846f

/* Returns the sign of a floating-point value:
 *    -  1.0f if value > 0
 *    - -1.0f if value < 0
 *    -  0.0f if value is +/-0
 *    - NAN   if value is NAN
 */
BAS_GEODEF float bas_scalar_signf(float value);

/* Converts degrees to radians. */
BAS_GEODEF float bas_scalar_deg_to_rad(float degrees);

/* Converts radians to degrees. */
BAS_GEODEF float bas_scalar_rad_to_deg(float radians);

/* Returns value clamped to the range [min, max]. */
BAS_GEODEF float bas_scalar_clamp(float value, float min, float max);

/* Returns t clamped to the range [0, 1]. */
BAS_GEODEF float bas_scalar_clamp01(float t);

/* Returns true if the absolute difference between a and b is less than
 * epsilon. If either value is NaN, returns false. epsilon must be
 * non-negative. */
BAS_GEODEF bool bas_scalar_approx_eq(float a, float b, float epsilon);

/* Linearly interpolates between a and b.
 *    t = 0 returns a.
 *    t = 1 returns b.
 * Values of t outside [0, 1] extrapolate beyond [a, b]. */
BAS_GEODEF float bas_scalar_lerp(float a, float b, float t);

/* Linearly interpolates between a and b using t clamped to [0, 1].
 *    t <= 0 returns a.
 *    t >= 1 returns b. */
BAS_GEODEF float bas_scalar_lerp_clamped(float a, float b, float t);

/* Returns the interpolation factor t such that
 * bas_scalar_lerp(a, b, t) == value. Returns 0 when a == b. The result is
 * not clamped and may fall outside [0, 1] when value lies outside [a, b]. */
BAS_GEODEF float bas_scalar_inverse_lerp(float a, float b, float value);

/* Wraps value into the range [0, length).
 *
 * Unlike the % operator and fmod(), negative values wrap back into the
 * range rather than remaining negative:
 *
 *     bas_scalar_wrap( 7.0f, 5.0f) == 2.0f
 *     bas_scalar_wrap(-1.0f, 5.0f) == 4.0f
 */
BAS_GEODEF float bas_scalar_wrap(float value, float length);

/* Smoothly interpolates from 0.0f to 1.0f as x moves from edge0 to edge1. */
BAS_GEODEF float bas_scalar_smooth_step(float edge0, float edge1, float x);

/* ========================================================================= */
/* Vec2                                                                      */
/* ========================================================================= */

typedef struct bas_vec2 {
  float x;
  float y;
} bas_vec2;

/* Construct a vector from two components. */
#define BAS_VEC2(x, y) ((bas_vec2){(x), (y)})

/* The zero vector. */
#define BAS_VEC2_ZERO ((bas_vec2){0.0f, 0.0f})

/* Return true if a and b are within epsilon of each other componentwise. */
BAS_GEODEF bool bas_vec2_approx_eq(bas_vec2 a, bas_vec2 b, float epsilon);

/* Componentwise a + b. */
BAS_GEODEF bas_vec2 bas_vec2_add(bas_vec2 a, bas_vec2 b);

/* Componentwise a - b. */
BAS_GEODEF bas_vec2 bas_vec2_sub(bas_vec2 a, bas_vec2 b);

/* Scale a by a scalar factor. */
BAS_GEODEF bas_vec2 bas_vec2_scale(bas_vec2 a, float factor);

/* Dot product of a and b. */
BAS_GEODEF float bas_vec2_dot(bas_vec2 a, bas_vec2 b);

/* Euclidean length of a. */
BAS_GEODEF float bas_vec2_length(bas_vec2 a);

/* Squared length of a (cheaper than bas_vec2_length; no sqrt). */
BAS_GEODEF float bas_vec2_length_sq(bas_vec2 a);

/* Unit-length vector in the direction of a; returns the zero vector if a is
   the zero vector. */
BAS_GEODEF bas_vec2 bas_vec2_normalize(bas_vec2 a);

/* Componentwise negation of a. */
BAS_GEODEF bas_vec2 bas_vec2_negate(bas_vec2 a);

/* Distance between points a and b. */
BAS_GEODEF float bas_vec2_distance(bas_vec2 a, bas_vec2 b);

/* Squared distance between points a and b (cheaper; no sqrt). */
BAS_GEODEF float bas_vec2_distance_sq(bas_vec2 a, bas_vec2 b);

/* Linear interpolation from a to b by t, where t in [0, 1]. */
BAS_GEODEF bas_vec2 bas_vec2_lerp(bas_vec2 a, bas_vec2 b, float t);

/* 2D cross product (z component of the 3D cross of a and b). */
BAS_GEODEF float bas_vec2_cross(bas_vec2 a, bas_vec2 b);

/* ========================================================================= */
/* AABB (axis-aligned bounding box)                                          */
/* ========================================================================= */

typedef struct bas_aabb {
  bas_vec2 min;
  bas_vec2 max;
} bas_aabb;

/* Status returned by the bas_aabb_* constructors. */
typedef enum bas_aabb_result {
  BAS_AABB_OK = 0,
  BAS_AABB_INVALID_BOUNDS,
  BAS_AABB_INVALID_SIZE,
  BAS_AABB_EMPTY_POINTS,
  BAS_AABB_NAN_NOT_ALLOWED,
  BAS_AABB_NULL_NOT_ALLOWED,
} bas_aabb_result;

/* Initialize *out from explicit min/max corners. Requires min <= max
 * componentwise. */
BAS_GEODEF bas_aabb_result bas_aabb_init(bas_aabb *out, bas_vec2 min,
                                         bas_vec2 max);

/* Initialize *out from a center point and a (non-negative) size. */
BAS_GEODEF bas_aabb_result bas_aabb_from_center_and_size(bas_aabb *out,
                                                         bas_vec2 center,
                                                         bas_vec2 size);

/* Initialize *out as the tightest box enclosing count points. Requires
 * count > 0. */
BAS_GEODEF bas_aabb_result bas_aabb_from_points(bas_aabb *out,
                                                const bas_vec2 *points,
                                                size_t count);

#endif /* BAS_INCLUDE_BAS_GEOMETRY_H */


/* ------------------------------------------------------------------------- */
/* IMPLEMENTATION                                                            */
/* ------------------------------------------------------------------------- */

#ifdef BAS_GEOMETRY_IMPLEMENTATION

#include <assert.h>
#include <math.h>

/* ---- Scalar ------------------------------------------------------------- */

BAS_GEODEF float bas_scalar_signf(float value) {
  if (isnan(value))
    return NAN;
  if (value > 0.0f)
    return 1.0f;
  if (value < 0.0f)
    return -1.0f;
  return 0.0f;
}

BAS_GEODEF float bas_scalar_deg_to_rad(float degrees) {
  return degrees * (BAS_SCALAR_PI / 180.0f);
}

BAS_GEODEF float bas_scalar_rad_to_deg(float radians) {
  return radians * (180.0f / BAS_SCALAR_PI);
}

BAS_GEODEF float bas_scalar_clamp(float value, float min, float max) {
  return fminf(fmaxf(value, min), max);
}

BAS_GEODEF float bas_scalar_clamp01(float t) {
  return bas_scalar_clamp(t, 0.0f, 1.0f);
}

BAS_GEODEF bool bas_scalar_approx_eq(float a, float b, float epsilon) {
  if (isnan(a) || isnan(b)) {
    return false;
  }

  assert(epsilon >= 0.0f);

  return fabsf(a - b) < epsilon;
}

BAS_GEODEF float bas_scalar_lerp(float a, float b, float t) {
  return (1 - t) * a + t * b;
}

BAS_GEODEF float bas_scalar_lerp_clamped(float a, float b, float t) {
  float clamped_t = bas_scalar_clamp01(t);
  return a + (b - a) * clamped_t;
}

BAS_GEODEF float bas_scalar_inverse_lerp(float a, float b, float value) {
  if (a == b) {
    return 0.0f;
  }

  return (value - a) / (b - a);
}

BAS_GEODEF float bas_scalar_wrap(float value, float length) {
  assert(length > 0.0f);
  return value - length * floorf(value / length);
}

BAS_GEODEF float bas_scalar_smooth_step(float edge0, float edge1, float x) {
  const float t = bas_scalar_inverse_lerp(edge0, edge1, x);
  const float clamped_t = bas_scalar_clamp01(t);
  return clamped_t * clamped_t * (3.0f - 2.0f * clamped_t);
}

/* ---- Vec2 --------------------------------------------------------------- */

BAS_GEODEF bool bas_vec2_approx_eq(bas_vec2 a, bas_vec2 b, float epsilon) {
  return bas_scalar_approx_eq(a.x, b.x, epsilon) &&
         bas_scalar_approx_eq(a.y, b.y, epsilon);
}

BAS_GEODEF bas_vec2 bas_vec2_add(bas_vec2 a, bas_vec2 b) {
  return (bas_vec2){a.x + b.x, a.y + b.y};
}

BAS_GEODEF bas_vec2 bas_vec2_sub(bas_vec2 a, bas_vec2 b) {
  return (bas_vec2){a.x - b.x, a.y - b.y};
}

BAS_GEODEF bas_vec2 bas_vec2_scale(bas_vec2 a, float factor) {
  return (bas_vec2){a.x * factor, a.y * factor};
}

BAS_GEODEF float bas_vec2_dot(bas_vec2 a, bas_vec2 b) {
  return a.x * b.x + a.y * b.y;
}

BAS_GEODEF float bas_vec2_length_sq(bas_vec2 a) { return bas_vec2_dot(a, a); }

BAS_GEODEF float bas_vec2_length(bas_vec2 a) {
  return sqrtf(bas_vec2_length_sq(a));
}

BAS_GEODEF bas_vec2 bas_vec2_normalize(bas_vec2 a) {
  const float len = bas_vec2_length(a);

  if (len == 0.0f) {
    return BAS_VEC2_ZERO;
  }

  return bas_vec2_scale(a, 1.0f / len);
}

BAS_GEODEF bas_vec2 bas_vec2_negate(bas_vec2 a) {
  return bas_vec2_scale(a, -1.0f);
}

BAS_GEODEF float bas_vec2_distance(bas_vec2 a, bas_vec2 b) {
  return bas_vec2_length(bas_vec2_sub(a, b));
}

BAS_GEODEF float bas_vec2_distance_sq(bas_vec2 a, bas_vec2 b) {
  return bas_vec2_length_sq(bas_vec2_sub(a, b));
}

BAS_GEODEF bas_vec2 bas_vec2_lerp(bas_vec2 a, bas_vec2 b, float t) {
  return (bas_vec2){
      bas_scalar_lerp(a.x, b.x, t),
      bas_scalar_lerp(a.y, b.y, t),
  };
}

BAS_GEODEF float bas_vec2_cross(bas_vec2 a, bas_vec2 b) {
  return a.x * b.y - a.y * b.x;
}

/* ---- AABB --------------------------------------------------------------- */

BAS_GEODEF bas_aabb_result bas_aabb_init(bas_aabb *out, bas_vec2 min,
                                         bas_vec2 max) {
  if (out == NULL) {
    return BAS_AABB_NULL_NOT_ALLOWED;
  }
  if (min.x > max.x || min.y > max.y) {
    return BAS_AABB_INVALID_BOUNDS;
  }

  out->min = min;
  out->max = max;

  return BAS_AABB_OK;
}

BAS_GEODEF bas_aabb_result bas_aabb_from_center_and_size(bas_aabb *out,
                                                         bas_vec2 center,
                                                         bas_vec2 size) {
  if (out == NULL) {
    return BAS_AABB_NULL_NOT_ALLOWED;
  }
  if (size.x < 0.0f || size.y < 0.0f) {
    return BAS_AABB_INVALID_SIZE;
  }

  if (isnan(size.x) || isnan(size.y)) {
    return BAS_AABB_NAN_NOT_ALLOWED;
  }

  const bas_vec2 half_size = bas_vec2_scale(size, 0.5f);

  out->min = bas_vec2_sub(center, half_size);
  out->max = bas_vec2_add(center, half_size);

  return BAS_AABB_OK;
}

BAS_GEODEF bas_aabb_result bas_aabb_from_points(bas_aabb *out,
                                                const bas_vec2 *points,
                                                size_t count) {
  if (out == NULL) {
    return BAS_AABB_NULL_NOT_ALLOWED;
  }
  if (count == 0) {
    return BAS_AABB_EMPTY_POINTS;
  }

  bas_vec2 min = points[0];
  bas_vec2 max = points[0];

  for (size_t i = 1; i < count; i++) {
    const bas_vec2 point = points[i];

    if (point.x < min.x)
      min.x = point.x;
    if (point.y < min.y)
      min.y = point.y;
    if (point.x > max.x)
      max.x = point.x;
    if (point.y > max.y)
      max.y = point.y;
  }

  out->min = min;
  out->max = max;

  return BAS_AABB_OK;
}

#endif /* BAS_GEOMETRY_IMPLEMENTATION */
