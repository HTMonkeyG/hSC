#ifndef __AABB_H__
#define __AABB_H__

#include "aliases.h"
#include "mth/vector.h"

#ifdef __cplusplus
extern "C" {
#endif

// 3D AABB, the fourth component of the v4f is disabled.
typedef struct {
  v4f lower;
  v4f upper;
} AABB_t;

AABB_t *aabb_fromPoints(AABB_t *aabb, v4f p1, v4f p2);
AABB_t *aabb_fromCenter(AABB_t *aabb, v4f center, v4f sides);
AABB_t *aabb_getAllVertices(AABB_t *aabb, v4f points[8]);

v4f aabb_getBounds(AABB_t *aabb);
v4f aabb_getCenter(AABB_t *aabb);

f32 aabb_getVolume(AABB_t *aabb);

#ifdef __cplusplus
}
#endif

#endif
