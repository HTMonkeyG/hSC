#ifndef __MATRIX_H__
#define __MATRIX_H__

#include <immintrin.h>

#include "internal.h"
#include "mth/vector.h"

#ifdef __cplusplus
extern "C" {
#endif

#define m44 M44

typedef union {
  f32 m[16];
  f32 n[4][4];
  v4f rows[4];
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
 * @param r
 * @param a
 * @param b
 * @returns
 */
static inline m44 *m44mul(m44 *r, m44 *a, m44 *b) {
  m44 t = {0};

  // This function is unoptimized.
  for (i32 i = 0; i < 4; i++)
    for (i32 j = 0; j < 4; j++)
      for (i32 k = 0; k < 4; k++)
        t.n[i][j] += a->n[i][k] * b->n[k][j];

  memcpy(r, &t, sizeof(m44));

  return r;
}

#ifdef __cplusplus
}
#endif

#endif
