#ifndef __FPV_H__
#define __FPV_H__

#include "internal.h"

#ifdef __cplusplus
extern "C" {
#endif

#define FPVRST_POS (0x01)
#define FPVRST_ROT (0x02)

// The type of the fpv update functions must match the definition below.
typedef i08 (__fastcall *PFN_FpvUpdate)(
  v4f, v4f, f32);
typedef i08 (__fastcall *PFN_FpvReset)(
  v4f, v4f, i32);

typedef struct {
  // -- User-defined flags.
  u32 flags;

  // -- Movement.
  // Position.
  v4f pos;
  // Velocity.
  v4f vel;
  // Acceleration.
  v4f acc;

  // -- Rotation.
  // Rotation.
  v4f rot;
  // Angular velocity.
  v4f avel;
  // Angular acceleration.
  v4f aacc;

  // Rotation matrix and position vector, the final output. The update
  // function must convert `rot` and `pos` into this field.
  m44 matrix;
} HscFpv;

i08 hscFpvCheckCollision(
  AABB_t *aabb,
  v4f *velocity,
  f32 timeElapsed);

#ifdef __cplusplus
}
#endif

#endif
