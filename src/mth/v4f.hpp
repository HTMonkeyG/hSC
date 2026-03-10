#ifndef __V4F_HPP__
#define __V4F_HPP__

#include <intrin.h>
#include <iostream>
#include "aliases.h"

struct v4f;

static inline v4f v4fnew(
  f32 x, f32 y, f32 z, f32 w);

static inline v4f v4fadd(
  v4f a, v4f b);
static inline v4f v4fsub(
  v4f a, v4f b);
static inline v4f v4fmul(
  v4f a, v4f b);
static inline v4f v4fdiv(
  v4f a, v4f b);

static inline v4f v4fscale(
  v4f a, f32 s);
static inline v4f v4fabs(
  v4f a);
static inline v4f v4fnormalize(
  v4f a);

static inline v4f v4freflect(
  v4f n, v4f i);
static inline v4f v4fprojection(
  v4f a, v4f b);

static inline f32 v4fdot(
  v4f a, v4f b);
static inline f32 v4flen(
  v4f a);

static inline v4f v4fblend(
  v4f a, v4f b, i32 mask);
static inline v4f v4fseleq(
  v4f x, v4f a, v4f b);
static inline v4f v4fselgt(
  v4f x, v4f a, v4f b);

#ifndef __cplusplus

// C Vec4 inplementation.
typedef struct {
  union {
    f32 n[4];
    __m128 m128;
    struct {
      f32 x;
      f32 y;
      f32 z;
      f32 w;
    };
  };
} v4f;

#else

// C++ Vec4 inplementation.
struct v4f {
  // Calculate reflection vector.
  static inline v4f reflect(
    v4f n,
    v4f i
  ) {
    return v4freflect(n, i);
  }

  // Calculate the projection vector of a on b.
  static inline v4f projection(
    v4f a,
    v4f b
  ) {
    return v4fprojection(a, b);
  }

  // Performs an element-by-element blending with the given mask. Only the
  // lowest 4 bit of the mask is used.
  static inline v4f blend(
    v4f a,
    v4f b,
    i32 mask
  ) {
    return v4fblend(a, b, mask);
  }

  // When the component of vector x is not 0, select the corresponding component
  // of a, otherwise select b.
  static inline v4f seleq(
    v4f x,
    v4f a,
    v4f b
  ) {
    return v4fseleq(x, a, b);
  }

  // When the component of vector x is larger than 0, select the corresponding
  // component of a, otherwise select b.
  static inline v4f selgt(
    v4f x,
    v4f a,
    v4f b
  ) {
    return v4fselgt(x, a, b);
  }

  /**
   * Calculate the dot product of vector a and b.
   */
  static inline f32 dot(
    v4f a,
    v4f b
  ) {
    return v4fdot(a, b);
  }

  /* Constructors. */

  // Default constructor, zero vector.
  inline v4f(): m128(_mm_set1_ps(0)) { }

  // Construct a vector with the same components.
  inline v4f(f32 v): m128(_mm_set1_ps(v)) { }

  // Construct a vector with a __m128.
  inline v4f(__m128 c): m128(c) { }

  // Construct a vector.
  inline v4f(f32 x, f32 y, f32 z, f32 w): x(x), y(y), z(z), w(w) { }

  // Elements.
  union {
    f32 n[4];
    __m128 m128;
    struct {
      f32 x;
      f32 y;
      f32 z;
      f32 w;
    };
  };

  /* Stream operations. */

  inline friend std::ostream &operator<<(
    std::ostream &out,
    const v4f &a
  ) {
    return out << "(" << a.x << "," << a.y << "," << a.z << "," << a.w << ")";
  }

  /* Per-element operations. */

  // Multiply each component by -1.
  inline v4f operator-() {
    return _mm_mul_ps(m128, _mm_set1_ps(-1.0f));
  }

  // Add b to a.
  inline friend v4f operator+(
    const v4f &a,
    const v4f &b
  ) {
    return _mm_add_ps(a.m128, b.m128);
  }

  // Add a vector.
  inline v4f &operator+=(
    const v4f &v
  ) {
    m128 = _mm_add_ps(m128, v.m128);
    return *this;
  }

  // Subtract b from a.
  inline friend v4f operator-(
    const v4f &a,
    const v4f &b
  ) {
    return _mm_sub_ps(a.m128, b.m128);
  }

  // Subtract a vector.
  inline v4f &operator-=(
    const v4f &v
  ) {
    m128 = _mm_sub_ps(m128, v.m128);
    return *this;
  }

  // Multiply each component of two vectors.
  inline friend v4f operator*(
    const v4f &a,
    const v4f &b
  ) {
    return _mm_mul_ps(a.m128, b.m128);
  }

  // Multiply a vector.
  inline v4f &operator*=(
    const v4f &v
  ) {
    m128 = _mm_mul_ps(m128, v.m128);
    return *this;
  }

  // Divide the components of a by b.
  inline friend v4f operator/(
    const v4f &a,
    const v4f &b
  ) {
    return _mm_div_ps(a.m128, b.m128);
  }

  // Divide by a vector.
  inline v4f &operator/=(
    const v4f &v
  ) {
    m128 = _mm_div_ps(m128, v.m128);
    return *this;
  }

  // Returns true when a vector is zero vector.
  inline bool operator!() {
    return !len();
  }

  // Return the absolute value of each component of a.
  inline v4f abs() {
    return v4fabs(*this);
  }

  // Normalize the vector a.
  inline v4f normalize() {
    float l = len();

    if (l > 0)
      return v4f{m128} * (1.0f / l);

    return v4f{0.0f};
  }

  /* Vector operations. */

  // The length of the vector.
  inline f32 len() {
    return v4flen(*this);
  }
};

// #ifdef __cplusplus
#endif

static const v4f V4FZERO = {0};

// Create a new v4f.
static inline v4f v4fnew(
  f32 x,
  f32 y,
  f32 z,
  f32 w
) {
  v4f r;

  r.x = x;
  r.y = y;
  r.z = z;
  r.w = w;

  return r;
}

// Add b to a.
static inline v4f v4fadd(
  v4f a,
  v4f b
) {
  v4f r;
  r.m128 = _mm_add_ps(a.m128, b.m128);
  return r;
}

// Subtract b from a.
static inline v4f v4fsub(
  v4f a,
  v4f b
) {
  v4f r;
  r.m128 = _mm_sub_ps(a.m128, b.m128);
  return r;
}

// Multiply each component of two vectors.
static inline v4f v4fmul(
  v4f a,
  v4f b
) {
  v4f r;
  r.m128 = _mm_mul_ps(a.m128, b.m128);
  return r;
}

// Divide the components of a by b.
static inline v4f v4fdiv(
  v4f a,
  v4f b
) {
  v4f r;
  r.m128 = _mm_div_ps(a.m128, b.m128);
  return r;
}

// Multiply s to each component of the vector a.
static inline v4f v4fscale(
  v4f a,
  f32 s
) {
  __m128 v1;
  v4f r;

  v1 = _mm_set1_ps(s);
  r.m128 = _mm_mul_ps(a.m128, v1);

  return r;
}

// Normalize the vector a.
static inline v4f v4fnormalize(
  v4f a
) {
  float l = v4flen(a);

  if (l > 0)
    return v4fscale(a, 1.0f / l);

  return 0.0f;
}

// Return the absolute value of each component of a.
static inline v4f v4fabs(
  v4f a
) {
  __m128 vMask;
  v4f r;
  static const u32 mask[4] = {
    0x7FFFFFFF,
    0x7FFFFFFF,
    0x7FFFFFFF,
    0x7FFFFFFF
  };

  vMask = _mm_loadu_ps((float *)mask);
  r.m128 = _mm_and_ps(a.m128, vMask);

  return r;
}

// Calculate reflection vector.
static inline v4f v4freflect(
  v4f n,
  v4f i
) {
  __m128 v1;
  v4f r;

  n = v4fnormalize(n);

  // dot(i, n)
  v1 = _mm_dp_ps(i.m128, n.m128, 0xFF);
  // 2.0f * dot(i, n)
  v1 = _mm_mul_ps(v1, _mm_set1_ps(2.0f));
  // scale * n
  v1 = _mm_mul_ps(v1, n.m128);
  // i - scaled_n
  v1 = _mm_sub_ps(i.m128, v1);

  r.m128 = v1;
  return r;
}

// Calculate the projection vector of a on b.
static inline v4f v4fprojection(
  v4f a,
  v4f b
) {
  __m128 v1, v2, v3, zero, mask, scale;
  v4f r;

  v1 = a.m128;
  v2 = b.m128;

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

  r.m128 = v1;

  return r;
}

// Performs an element-by-element blending with the given mask. Only the
// lowest 4 bit of the mask is used.
static inline v4f v4fblend(
  v4f a,
  v4f b,
  i32 mask
) {
  v4f r;
  __m128 v1 = _mm_setr_ps(
    (mask & 0x1) ? -0.0f : 0.0f,
    (mask & 0x2) ? -0.0f : 0.0f,
    (mask & 0x4) ? -0.0f : 0.0f,
    (mask & 0x8) ? -0.0f : 0.0f
  );

  r.m128 = _mm_blendv_ps(a.m128, b.m128, v1);

  return r;
}

// When the component of vector x is not 0, select the corresponding component
// of a, otherwise select b.
static inline v4f v4fseleq(
  v4f x,
  v4f a,
  v4f b
) {
  v4f r;
  __m128 v1, v2, v3, mask;

  v1 = x.m128;
  v2 = a.m128;
  v3 = b.m128;

  mask = _mm_cmpneq_ps(v1, _mm_setzero_ps());

  v1 = _mm_or_ps(
    _mm_and_ps(mask, v2),
    _mm_andnot_ps(mask, v3)
  );
  r.m128 = v1;

  return r;
}

// When the component of vector x is larger than 0, select the corresponding
// component of a, otherwise select b.
static inline v4f v4fselgt(
  v4f x,
  v4f a,
  v4f b
) {
  v4f r;
  __m128 v1, v2, v3, mask;

  v1 = x.m128;
  v2 = a.m128;
  v3 = b.m128;

  mask = _mm_cmpgt_ps(v1, _mm_setzero_ps());
    
  v1 = _mm_or_ps(
    _mm_and_ps(mask, v2),
    _mm_andnot_ps(mask, v3)
  );
  r.m128 = v1;

  return r;
}

// Calculate the dot product of vector a and b.
static inline f32 v4fdot(
  v4f a,
  v4f b
) {
  return _mm_cvtss_f32(_mm_dp_ps(a.m128, b.m128, 0xFF));
}

// Get the length of the vector a.
static inline f32 v4flen(
  v4f a
) {
  __m128 v1, v2;

  v1 = a.m128;
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

// #ifndef __V4F_HPP__
#endif
