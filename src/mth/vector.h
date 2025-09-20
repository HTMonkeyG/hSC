#ifndef __VECTOR_H__
#define __VECTOR_H__

#include <intrin.h>

#include "aliases.h"

#ifdef __cplusplus
extern "C" {
#endif

#define v2f V2F
#define v3f V3F
#define v4f V4F
#define v2d V2D
#define v3d V3D
#define v4d V4D

typedef struct {
  f32 x;
  f32 y;
} V2F;

typedef struct {
  f32 x;
  f32 y;
  f32 z;
} V3F;

typedef struct {
  f32 x;
  f32 y;
  f32 z;
  f32 w;
} V4F;

typedef struct {
  f64 x;
  f64 y;
} V2D;

typedef struct {
  f64 x;
  f64 y;
  f64 z;
} V3D;

typedef struct {
  f64 x;
  f64 y;
  f64 z;
  f64 w;
} V4D;

static const v4f V4FZERO = {0};

static inline v4f v4fnew(f32 x, f32 y, f32 z, f32 w);
static inline v4f v4fadd(v4f a, v4f b);
static inline v4f v4fsub(v4f a, v4f b);
static inline v4f v4fmul(v4f a, v4f b);
static inline v4f v4fdiv(v4f a, v4f b);
static inline v4f v4fscale(v4f a, f32 s);
static inline v4f v4fnormalize(v4f a);
static inline v4f v4freflect(v4f n, v4f i);
static inline v4f v4fprojection(v4f a, v4f b);
static inline f32 v4fdot(v4f a, v4f b);
static inline f32 v4flen(v4f a);

static inline v4f v4fnew(f32 x, f32 y, f32 z, f32 w) {
  v4f r;

  r.x = x;
  r.y = y;
  r.z = z;
  r.w = w;

  return r;
}

/**
 * Add b to a.
 * 
 * @param a
 * @param b
 * @returns
 */
static inline v4f v4fadd(v4f a, v4f b) {
  __m128 v1, v2;
  v4f r;

  v1 = _mm_loadu_ps((float *)&a);
  v2 = _mm_loadu_ps((float *)&b);

  v1 = _mm_add_ps(v1, v2);

  _mm_storeu_ps((float *)&r, v1);

  return r;
}

/**
 * Subtract b from a.
 * 
 * @param a
 * @param b
 * @returns
 */
static inline v4f v4fsub(v4f a, v4f b) {
  __m128 v1, v2;
  v4f r;

  v1 = _mm_loadu_ps((float *)&a);
  v2 = _mm_loadu_ps((float *)&b);

  v1 = _mm_sub_ps(v1, v2);

  _mm_storeu_ps((float *)&r, v1);

  return r;
}

/**
 * Multiply each component of two vectors.
 * 
 * @param a
 * @param b
 * @returns
 */
static inline v4f v4fmul(v4f a, v4f b) {
  __m128 v1, v2;
  v4f r;

  v1 = _mm_loadu_ps((float *)&a);
  v2 = _mm_loadu_ps((float *)&b);

  v1 = _mm_mul_ps(v1, v2);

  _mm_storeu_ps((float *)&r, v1);

  return r;
}

/**
 * Divide the components of a by b.
 * 
 * @param a
 * @param b
 * @returns
 */
static inline v4f v4fdiv(v4f a, v4f b) {
  __m128 v1, v2;
  v4f r;

  v1 = _mm_loadu_ps((float *)&a);
  v2 = _mm_loadu_ps((float *)&b);

  v1 = _mm_div_ps(v1, v2);

  _mm_storeu_ps((float *)&r, v1);

  return r;
}

/**
 * Multiply s to each component of the vector a.
 * 
 * @param a
 * @param s
 * @returns
 */
static inline v4f v4fscale(v4f a, f32 s) {
  __m128 v1, v2;
  v4f r;

  v1 = _mm_loadu_ps((float *)&a);

  v2 = _mm_set1_ps(s);
  v1 = _mm_mul_ps(v1, v2);

  _mm_storeu_ps((float *)&r, v1);

  return r;
}

/**
 * Normalize the vector a.
 * 
 * @param a
 * @returns
 */
static inline v4f v4fnormalize(v4f a) {
  float l = v4flen(a);

  if (l > 0)
    return v4fscale(a, 1.0f / l);

  return a;
}

/**
 * Return the absolute value of each component of a.
 * 
 * @param a
 * @returns
 */
static inline v4f v4fabs(v4f a) {
  __m128 v1, vMask;
  v4f r;
  static const u32 mask[4] = {
    0x7FFFFFFF,
    0x7FFFFFFF,
    0x7FFFFFFF,
    0x7FFFFFFF
  };

  v1 = _mm_loadu_ps((float *)&a);
  vMask = _mm_loadu_ps((float *)mask);

  v1 = _mm_and_ps(v1, vMask);

  _mm_storeu_ps((float *)&r, v1);

  return r;
}

/**
 * Calculate reflection vector.
 * 
 * @param n Normalize vector.
 * @param i Incident vector.
 */
static inline v4f v4freflect(v4f n, v4f i) {
  __m128 v1, v2, v3;
  v4f r;

  n = v4fnormalize(n);
  v1 = _mm_loadu_ps((float *)&n);
  v2 = _mm_loadu_ps((float *)&i);

  // dot(i, n)
  v3 = _mm_dp_ps(v1, v2, 0xFF);
  // 2.0f * dot(i, n)
  v3 = _mm_mul_ps(v3, _mm_set1_ps(2.0f));
  // scale * n
  v3 = _mm_mul_ps(v3, v1);
  // i - scaled_n
  v3 = _mm_sub_ps(v2, v3);

  _mm_storeu_ps((float *)&r, v3);
  return r;
}
/**
 * Calculate the projection vector of a on b.
 * 
 * @param a
 * @param b
 */
static inline v4f v4fprojection(v4f a, v4f b) {
  __m128 v1, v2, v3, zero, mask, scale;
  v4f r;

  v1 = _mm_loadu_ps((float *)&a);
  v2 = _mm_loadu_ps((float *)&b);

  // dot(a, b)
  v1 = _mm_dp_ps(v1, v2, 0xFF);
  // dot(b, b) = |b|^2
  v3 = _mm_dp_ps(v2, v2, 0xFF);
  zero = _mm_setzero_ps();
  // scale = dot(a, b) / dot(b, b)
  scale = _mm_div_ps(v1, v3);
  // Avoid divide by 0.
  mask = _mm_cmpneq_ps(v3, zero);
  scale = _mm_and_ps(scale, mask);
  // Scale b.
  v1 = _mm_mul_ps(scale, v2);

  _mm_storeu_ps((float *)&r, v1);

  return r;
}

/**
 * When the component of vector x is not 0, select the corresponding component
 * of a, otherwise select b.
 * 
 * @param x
 * @param a
 * @param b
 */
static inline v4f v4fseleq(v4f x, v4f a, v4f b) {
  v4f r;
  __m128 v1, v2, v3, mask;

  v1 = _mm_loadu_ps((float *)&x);
  v2 = _mm_loadu_ps((float *)&a);
  v3 = _mm_loadu_ps((float *)&b);

  mask = _mm_cmpneq_ps(v1, _mm_setzero_ps());

  v1 = _mm_or_ps(
    _mm_and_ps(mask, v2),
    _mm_andnot_ps(mask, v3)
  );
  _mm_storeu_ps((float *)&r, v1);

  return r;
}

/**
 * When the component of vector x is larger than 0, select the corresponding
 * component of a, otherwise select b.
 * 
 * @param x
 * @param a
 * @param b
 */
static inline v4f v4fselgt(v4f x, v4f a, v4f b) {
  v4f r;
  __m128 v1, v2, v3, mask;

  v1 = _mm_loadu_ps((float *)&x);
  v2 = _mm_loadu_ps((float *)&a);
  v3 = _mm_loadu_ps((float *)&b);

  mask = _mm_cmpgt_ps(v1, _mm_setzero_ps());
    
  v1 = _mm_or_ps(
    _mm_and_ps(mask, v2),
    _mm_andnot_ps(mask, v3)
  );
  _mm_storeu_ps((float *)&r, v1);

  return r;
}

/**
 * Calculate the dot product of vector a and b.
 * 
 * @param a
 * @param b
 * @returns
 */
static inline f32 v4fdot(v4f a, v4f b) {
  __m128 v1, v2;

  v1 = _mm_loadu_ps((float *)&a);
  v2 = _mm_loadu_ps((float *)&b);

  v1 = _mm_dp_ps(v1, v2, 0xFF);

  return _mm_cvtss_f32(v1);
}

/**
 * Get the length of the vector a.
 * 
 * @param a
 * @returns
 */
static inline f32 v4flen(v4f a) {
  __m128 v1, v2;

  v1 = _mm_loadu_ps((float *)&a);
  // Square each components.
  v1 = _mm_mul_ps(v1, v1);
  // Copy the first and the third component.
  // v1: {x^2, y^2, z^2, w^2}
  // v2: {x^2, x^2, z^2, z^2}
  v2 = _mm_movehdup_ps(v1);
  // Add two vectors.
  // v2: {-, x^2 + y^2, -, z^2 + w^2}
  v2 = _mm_add_ps(v1, v2);
  // Duplicate the first two f32.
  // v1: {-, -, -, x^2 + y^2}
  v1 = (__m128)_mm_permute_pd((__m128d)v2, 1);
  // Get the final result.
  // v1: {-, -, -, x^2 + y^2 + z^2 + w^2}
  v1 = _mm_add_ss(v1, v2);
  v1 = _mm_sqrt_ss(v1);

  return _mm_cvtss_f32(v1);
}

#ifdef __cplusplus
}
#endif

#endif
