#include "aabb.h"
#include "macros.h"

AABB_t *aabb_fromPoints(AABB_t *aabb, v4f p1, v4f p2) {
  if (!aabb)
    return NULL;

  aabb->lower.x = m_min(p1.x, p2.x);
  aabb->lower.y = m_min(p1.y, p2.y);
  aabb->lower.z = m_min(p1.z, p2.z);

  aabb->upper.x = m_max(p1.x, p2.x);
  aabb->upper.y = m_max(p1.y, p2.y);
  aabb->upper.z = m_max(p1.z, p2.z);

  return aabb;
}

AABB_t *aabb_fromCenter(AABB_t *aabb, v4f center, v4f sides) {
  if (!aabb)
    return NULL;

  aabb->lower = v4fsub(center, v4fscale(sides, 0.5f));
  aabb->upper = v4fadd(center, v4fscale(sides, 0.5f));

  return aabb;
}

AABB_t *aabb_getAllVertices(AABB_t *aabb, v4f points[8]) {
  if (!aabb || !points)
    return NULL;

  for (int i = 0; i < 8; i++) {
    points[i].x = i & 0x1 ? aabb->lower.x : aabb->upper.x;
    points[i].y = i & 0x2 ? aabb->lower.y : aabb->upper.y;
    points[i].z = i & 0x4 ? aabb->lower.z : aabb->upper.z;
  }

  return aabb;
}

v4f aabb_getBounds(AABB_t *aabb) {
  v4f result = {0};

  if (!aabb)
    return result;

  result.x = aabb->upper.x - aabb->lower.x;
  result.y = aabb->upper.y - aabb->lower.y;
  result.z = aabb->upper.z - aabb->lower.z;

  return result;
}

v4f aabb_getCenter(AABB_t *aabb) {
  v4f result = {0};

  if (!aabb)
    return result;
  
  result = v4fscale(v4fadd(aabb->lower, aabb->upper), 0.5f);

  return result;
}

f32 aabb_getVolume(AABB_t *aabb) {
  if (!aabb)
    return 0.0f;
  return (aabb->upper.x - aabb->lower.x) * (aabb->upper.y - aabb->lower.y) * (aabb->upper.z - aabb->lower.z);
}
