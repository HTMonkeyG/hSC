#include "fpv.h"

/**
 * Encapsulation for invocations of World::interactionCheck().
 * 
 * No need to explicitly pass in the context parameter.
 * 
 * The pointer passed into this function must be a local variable address.
 */
i08 hscFpvCheckCollision(
  AABB_t *aabb,
  v4f *velocity,
  f32 timeElapsed
) {
  u08 result = 0
    , done = 0;
  v4f vertices[8]
    , origin, dir, center, vec;
  f32 len;
  i32 iter;
  RaycastResult ir;

  if (
    !gCollisionGeoBarn
    || !gTramp.fn_CollisionGeoBarn_raycast
    || !velocity
    || !aabb
  )
    return 0;

  vec = *velocity;
  center = aabb_getCenter(aabb);
  aabb_getAllVertices(aabb, vertices);
  dir = v4fnormalize(vec);
  len = v4flen(vec) * timeElapsed;

  for (i32 i = 0; i < 8; i++) {
    // Iterate 8 vertices of the AABB.
    iter = 0;
    origin = vertices[i];

    if (v4fdot(vec, v4fsub(origin, center)) < 0.0f)
      // Skip vertices which is on the "back" of the AABB.
      continue;

    while (
      ((PFN_CollisionGeoBarn_raycast)gTramp.fn_CollisionGeoBarn_raycast)(
        gCollisionGeoBarn, &origin, &dir, len, NULL, (i08 *)&ir)
    ) {
      if (iter >= 4) {
        // If it still collides after 4 checks, then directly set the delta to
        // 0.
        memset(&vec, 0, sizeof(vec));
        break;
      }

      // Subtract the projection of the displacement vector onto the normal
      // vector to obtain the tangential component of the displacement vector.
      vec = v4fsub(vec, v4fprojection(vec, ir.normal));
      // Use the tangential vector to iterate the calculation 3 times, to avoid
      // corruptions at the intersection of two faces.
      dir = v4fnormalize(vec);
      len = v4flen(vec) * timeElapsed;

      result = 1;
      if (len < 0.0001) {
        // Directly break the loop if the displacement is small enough.
        done = 1;
        break;
      }

      iter++;
    }

    if (done)
      break;
  }

  *velocity = vec;

  return result;
}

void hscFpvBarnRegister() {

}
