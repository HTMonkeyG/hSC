#ifndef __EULER_H__
#define __EULER_H__

#include <math.h>
#include "mth/macros.h"
#include "mth/vector.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Calculate rotation matrix from euler angle, in the order of yaw, pitch,
 * roll.
 * 
 * @param euler The x, y and z component of this vector indicates the yaw,
 * pitch and roll angles.
 * @param matrix The 3x3 rotation matrix stored in three v4f in rows.
 */
static inline void eulerToRotationXYZ(v4f euler, v4f *matrix) {
  f32 cy = cos(euler.x)
    , sy = sin(euler.x)
    , cp = cos(euler.y)
    , sp = sin(euler.y)
    , cr = cos(euler.z)
    , sr = sin(euler.z);
  
  // R00
  matrix[0].x = cr * cy + sr * sp * sy;
  // R01
  matrix[0].y = sr * cp;
  // R02
  matrix[0].z = -cr * sy + sr * sp * cy;
  
  // R10
  matrix[1].x = cr * sp * sy - sr * cy;
  // R11
  matrix[1].y = cr * cp;
  // R12
  matrix[1].z = sr * sy + cr * sp * cy; 
  
  // R20
  matrix[2].x = cp * sy;
  // R21
  matrix[2].y = -sp;
  // R22
  matrix[2].z = cp * cy;

  matrix[0].w = matrix[1].w = matrix[2].w = 0;
}

/**
 * Calculate euler angle from rotation matrix, in the order of yaw, pitch,
 * roll.
 * 
 * @param euler The x, y and z component of this vector indicates the yaw,
 * pitch and roll angles.
 * @param matrix The 3x3 rotation matrix stored in three v4f in rows.
 */
static inline v4f *rotationToEulerXYZ(v4f *euler, v4f *matrix) {
  v4f dir = matrix[2]
    , result;

  // Get the pitch angle. When the camera looks "up", or, the foward vector
  // "raises", the pitch angle actually decreases, so there's a negative sign
  // here.
  result.y = -asinf(dir.y) / PI_F * 180.0f;

  // Get the yaw angle.
  if (dir.x == 0 && dir.z == 0)
    // Avoid dividing by 0, although this case won't happen in normal games.
    result.x = 0;
  else
    // There will be some floating point errors here, but it's fine.
    result.x = atan2f(dir.x, dir.z) / PI_F * 180.0f;

  // Get the roll angle. This value is always 0 in SkyCamera calls.
  if (matrix[0].y == 0 && matrix[1].y == 0)
    result.z = 0;
  else
    result.z = atan2f(matrix[0].y, matrix[1].y) / PI_F * 180.0f;

  *euler = result;
  return euler;
}

#ifdef __cplusplus
}
#endif

#endif
