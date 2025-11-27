#ifndef __MATRIX_H__
#define __MATRIX_H__

#include <immintrin.h>

#include "aliases.h"
#include "mth/vector.h"

#ifdef __cplusplus
extern "C" {
#endif

#define m44 M44

typedef union __attribute__((aligned(16))) {
  f32 m[16];
  f32 n[4][4];
  v4f rows[4];
  __m128 m128[4];
  // In Sky, all data is aligned to 16 bytes, so we removed this field.
  //__m256 m256[2];
  struct {
    v4f row1;
    v4f row2;
    v4f row3;
    v4f row4;
  };
  struct {
    f32 _11
      , _12
      , _13
      , _14
      , _21
      , _22
      , _23
      , _24
      , _31
      , _32
      , _33
      , _34
      , _41
      , _42
      , _43
      , _44;
  };
} M44;

/**
 * Multiply two matrices, R = A * B.
 * 
 * @param r The result.
 * @param a
 * @param b
 * @returns
 */
static inline m44 *m44mul(
  m44 *r,
  m44 *a,
  m44 *b
) {
  // Optimized version from <https://stackoverflow.com/a/46058667>.
  __m256 a0, a1, b0, b1;
  __m256 c0, c1, c2, c3, c4, c5, c6, c7;
  __m256 t0, t1, u0, u1;
  m44 t;

  // t0 = a00, a01, a02, a03, a10, a11, a12, a13
  // t1 = a20, a21, a22, a23, a30, a31, a32, a33
  // u0 = b00, b01, b02, b03, b10, b11, b12, b13
  // u1 = b20, b21, b22, b23, b30, b31, b32, b33
  t0 = _mm256_set_m128(a->m128[1], a->m128[0]);
  t1 = _mm256_set_m128(a->m128[3], a->m128[2]);
  u0 = _mm256_set_m128(b->m128[1], b->m128[0]);
  u1 = _mm256_set_m128(b->m128[3], b->m128[2]);

  // a0 = a00, a00, a00, a00, a10, a10, a10, a10
  // a1 = a20, a20, a20, a20, a30, a30, a30, a30
  // b0 = b00, b01, b02, b03, b00, b01, b02, b03
  // c0 = a00*b00  a00*b01  a00*b02  a00*b03  a10*b00  a10*b01  a10*b02  a10*b03
  // c1 = a20*b00  a20*b01  a20*b02  a20*b03  a30*b00  a30*b01  a30*b02  a30*b03
  a0 = _mm256_shuffle_ps(t0, t0, _MM_SHUFFLE(0, 0, 0, 0));
  a1 = _mm256_shuffle_ps(t1, t1, _MM_SHUFFLE(0, 0, 0, 0));
  b0 = _mm256_permute2f128_ps(u0, u0, 0x00);
  c0 = _mm256_mul_ps(a0, b0);
  c1 = _mm256_mul_ps(a1, b0);

  // a0 = a01, a01, a01, a01, a11, a11, a11, a11
  // a1 = a21, a21, a21, a21, a31, a31, a31, a31
  // b0 = b10, b11, b12, b13, b10, b11, b12, b13
  // c2 = a01*b10  a01*b11  a01*b12  a01*b13  a11*b10  a11*b11  a11*b12  a11*b13
  // c3 = a21*b10  a21*b11  a21*b12  a21*b13  a31*b10  a31*b11  a31*b12  a31*b13
  a0 = _mm256_shuffle_ps(t0, t0, _MM_SHUFFLE(1, 1, 1, 1));
  a1 = _mm256_shuffle_ps(t1, t1, _MM_SHUFFLE(1, 1, 1, 1));
  b0 = _mm256_permute2f128_ps(u0, u0, 0x11);
  c2 = _mm256_mul_ps(a0, b0);
  c3 = _mm256_mul_ps(a1, b0);

  // a0 = a02, a02, a02, a02, a12, a12, a12, a12
  // a1 = a22, a22, a22, a22, a32, a32, a32, a32
  // b0 = b20, b21, b22, b23, b20, b21, b22, b23
  // c4 = a02*b20  a02*b21  a02*b22  a02*b23  a12*b20  a12*b21  a12*b22  a12*b23
  // c5 = a22*b20  a22*b21  a22*b22  a22*b23  a32*b20  a32*b21  a32*b22  a32*b23
  a0 = _mm256_shuffle_ps(t0, t0, _MM_SHUFFLE(2, 2, 2, 2));
  a1 = _mm256_shuffle_ps(t1, t1, _MM_SHUFFLE(2, 2, 2, 2));
  b1 = _mm256_permute2f128_ps(u1, u1, 0x00);
  c4 = _mm256_mul_ps(a0, b1);
  c5 = _mm256_mul_ps(a1, b1);

  // a0 = a03, a03, a03, a03, a13, a13, a13, a13
  // a1 = a23, a23, a23, a23, a33, a33, a33, a33
  // b0 = b30, b31, b32, b33, b30, b31, b32, b33
  // c6 = a03*b30  a03*b31  a03*b32  a03*b33  a13*b30  a13*b31  a13*b32  a13*b33
  // c7 = a23*b30  a23*b31  a23*b32  a23*b33  a33*b30  a33*b31  a33*b32  a33*b33
  a0 = _mm256_shuffle_ps(t0, t0, _MM_SHUFFLE(3, 3, 3, 3));
  a1 = _mm256_shuffle_ps(t1, t1, _MM_SHUFFLE(3, 3, 3, 3));
  b1 = _mm256_permute2f128_ps(u1, u1, 0x11);
  c6 = _mm256_mul_ps(a0, b1);
  c7 = _mm256_mul_ps(a1, b1);

  // c0 = c0 + c2 (two terms, first two rows)
  // c4 = c4 + c6 (the other two terms, first two rows)
  // c1 = c1 + c3 (two terms, second two rows)
  // c5 = c5 + c7 (the other two terms, second two rose)
  c0 = _mm256_add_ps(c0, c2);
  c4 = _mm256_add_ps(c4, c6);
  c1 = _mm256_add_ps(c1, c3);
  c5 = _mm256_add_ps(c5, c7);

  // Finally complete addition of all four terms and return the results.
  // n0 = a00*b00+a01*b10+a02*b20+a03*b30  a00*b01+a01*b11+a02*b21+a03*b31  a00*b02+a01*b12+a02*b22+a03*b32  a00*b03+a01*b13+a02*b23+a03*b33
  //      a10*b00+a11*b10+a12*b20+a13*b30  a10*b01+a11*b11+a12*b21+a13*b31  a10*b02+a11*b12+a12*b22+a13*b32  a10*b03+a11*b13+a12*b23+a13*b33
  // n1 = a20*b00+a21*b10+a22*b20+a23*b30  a20*b01+a21*b11+a22*b21+a23*b31  a20*b02+a21*b12+a22*b22+a23*b32  a20*b03+a21*b13+a22*b23+a23*b33
  //      a30*b00+a31*b10+a32*b20+a33*b30  a30*b01+a31*b11+a32*b21+a33*b31  a30*b02+a31*b12+a32*b22+a33*b32  a30*b03+a31*b13+a32*b23+a33*b33
  c0 = _mm256_add_ps(c0, c4);
  c1 = _mm256_add_ps(c1, c5);
  t.m128[0] = _mm256_castps256_ps128(c0);
  t.m128[1] = _mm256_extractf128_ps(c0, 1);
  t.m128[2] = _mm256_castps256_ps128(c1);
  t.m128[3] = _mm256_extractf128_ps(c1, 1);

  memcpy(r, &t, sizeof(m44));
  return r;
}

#ifdef __cplusplus
}
#endif

#endif
