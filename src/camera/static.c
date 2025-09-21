#include "ui/ui.h"
#include "internal.h"

#define OVERRIDE_2(cond, v1, v2) ((cond) ? ((v1) = (v2)) : ((v2) = (v1)))
#define OVERRIDE_3(cond, v11, v12, v2) ((cond) ? ((v11) = ((v12) = (v2))) : ((v2) = (v11)))

/**
 * Encapsulation for invocations of World::interactionCheck().
 * 
 * No need to explicitly pass in the context parameter.
 * 
 * The pointer passed into this function must be a local variable address.
 */
static i08 freecamCheckCollision(
  AABB_t *aabb,
  v4f *delta
) {
  u08 result = 0
    , done = 0;
  v4f vertices[8]
    , origin, dir, center, vec;
  f32 len;
  i32 iter;
  InteractionResult ir;

  if (
    !gSavedLevelContext
    || !gTramp.fn_Level_interactionTest
    || !delta
    || !aabb
  )
    return 0;

  vec = *delta;
  center = aabb_getCenter(aabb);
  aabb_getAllVertices(aabb, vertices);
  dir = v4fnormalize(vec);
  len = v4flen(vec);

  for (i32 i = 0; i < 8; i++) {
    // Iterate 8 vertices of the AABB.
    iter = 0;
    origin = vertices[i];

    if (v4fdot(vec, v4fsub(origin, center)) < 0.0f)
      // Skip vertices which is on the "back" of the AABB.
      continue;

    while (
      ((FnWorld_interactionTest)gTramp.fn_Level_interactionTest)(
        gSavedLevelContext, &origin, &dir, len, NULL, (i08 *)&ir)
    ) {
      if (iter >= 4) {
        // If it still collides after 4 checks, then directly set the delta to
        // 0.
        memset(&vec, 0, sizeof(vec));
        break;
      }

      // Subtract the projection of the displacement vector onto the normal
      // vector to obtain the tangential component of the displacement vector.
      vec = v4fsub(vec, v4fprojection(vec, ir.normalize));
      // Use the tangential vector to iterate the calculation 3 times, to avoid
      // corruptions at the intersection of two faces.
      dir = v4fnormalize(vec);
      len = v4flen(vec);

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

  *delta = vec;

  return result;
}

void preupdateSet(MainCamera *this) {
  
}

void preupdateFreecam(MainCamera *this) {
  v4f deltaRot = {0}
    , size = {0.1f, 0.1f, 0.1f, 0.1f}
    // Original direction vector (foward vector).
    , oDir = {0}
    , *dir
    , lastPos, delta, mouseDelta;
  f32 t;
  m44 mat = {0};
  AABB_t aabb;

  dir = &this->context1.mat3;

  if (gState.resetPosFlag) {
    gState.pos = gState.mat[3] = this->context1.cameraPos;
    /*gState.rot = v4fnew(
      this->rotateXAnim,
      this->rotateYAnim,
      0,
      0);*/
    gState.resetPosFlag = 0;
    eulerToRotationXYZ(gState.rot, gState.mat);
    return;
  }

  mouseDelta = gui_getFacingDeltaRad();

  // Calculte the direction vector parallel to xOz plane.
  lastPos = gState.pos;

  // FIXME: When directly looks up, the horizontal direction vector will be a
  // zero vector, so the camera won't move.
  oDir.x = dir->x;
  oDir.y = dir->z;
  oDir = v4fnormalize(oDir);

  if (gState.freecamMode == FC_ORIENT) {
    // Combine forward vector and left vector.
    delta = v4fnew(
      -gState.movementInput.x * +oDir.y,
      gState.movementInput.y,
      -gState.movementInput.x * -oDir.x,
      0.0f);
    delta = v4fadd(delta, v4fscale(*dir, -gState.movementInput.z));

    // Do not override the rotation matrix.
    gState.useMatrix = 0;
    gState.rot.z = 0;
  } else if (gState.freecamMode == FC_AXIAL) {
    // Rotate it by the movement input. Axial mode will ignore the roll angle.
    delta.x = oDir.x * -gState.movementInput.z + oDir.y * -gState.movementInput.x;
    delta.y = gState.movementInput.y;
    delta.z = oDir.y * -gState.movementInput.z - oDir.x * -gState.movementInput.x;

    // Do not override the rotation matrix.
    gState.useMatrix = 0;
    gState.rot.z = 0;
  } else if (gState.freecamMode == FC_FULLDIR) {
    // Calculate rotation delta from hooked data.
    deltaRot.z = gState.facingInput.z * gState.freecamRotateSpeed * gGui.timeElapsedSecond;
    deltaRot.x = -mouseDelta.x;
    deltaRot.y = mouseDelta.y;

    if (gOptions.freecam.swapRollYaw) {
      t = deltaRot.z;
      deltaRot.z = deltaRot.x;
      deltaRot.x = t;
    }

    // Calculate rotation matrix based on last frame.
    eulerToRotationXYZ(deltaRot, (v4f *)&mat);
    mat.row4 = v4fnew(0, 0, 0, 1);
    m44mul((m44 *)gState.mat, &mat, (m44 *)gState.mat);

    // Combine forward vector and left vector. The full-direction mode will use
    // the foward vector we calculated, instead of the game.
    delta = v4fscale(gState.mat[2], -gState.movementInput.z);
    delta = v4fnormalize(delta);

    // Override the rotation matrix to enable roll angle.
    gState.useMatrix = 1;
  }
  delta = v4fscale(delta, gState.freecamSpeed * gGui.timeElapsedSecond);

  if (gState.freecamCollision) {
    // Build AABB and check collision.
    aabb_fromCenter(&aabb, gState.pos, size);
    freecamCheckCollision(&aabb, &delta);
  }

  // Multiply by speed. Note that this function only affects the rotation matrix.
  gState.pos = v4fadd(
    lastPos,
    delta);
  gState.mat[3] = gState.pos;

  // Always override the position.
  gState.usePos = 1;
}

void preupdateStatic(MainCamera *this) {
  v4f *pos, *dir, *gsRot
    , rot;

  dir = &this->context1.mat3;
  // Pointer to original camera pos caculated by the game.
  pos = &this->context1.cameraPos;
  gsRot = &gState.rot;

  // Override coodinates or copy original camera pos to overlay.
  OVERRIDE_2(gState.overridePos, *pos, gState.pos);
  gState.mat[3] = *pos;

  if (gState.overrideDir) {
    // Override facing directions.
    rot = v4fscale(gState.rot, PI_F / 180.0f);
    // Copy the rotation matrix to gState. We must provide the matrix to the
    // gui.
    eulerToRotationXYZ(rot, gState.mat);
  } else {
    // Sync original facing directions to overlay. We assume that the rotation
    // matrix is in the order of yaw-pitch-roll, so we can apply the following
    // codes.

    // Get the pitch angle. When the camera looks "up", or, the foward vector
    // "raises", the pitch angle actually decreases, so there's a negative sign
    // here.
    gsRot->y = -asinf(dir->y) / PI_F * 180.0f;

    // Get the yaw angle.
    if (dir->x == 0 && dir->z == 0)
      // Avoid dividing by 0, although this case won't happen in normal games.
      gsRot->x = 0;
    else {
      // There will be some floating point errors here, but it's fine.
      gsRot->x = atan2f(dir->x, dir->z) / PI_F * 180.0f;
      gsRot->x += gsRot->x < 0 ? 360.0f : 0;
    }

    // Get the roll angle. This value is always 0 in SkyCamera calls.
    if (this->context1.mat1.y == 0 && this->context1.mat2.y == 0)
      gsRot->z = 0;
    else {
      gsRot->z = atan2f(this->context1.mat1.y, this->context1.mat2.y) / PI_F * 180.0f;
      gsRot->z += gsRot->z < 0 ? 360.0f : 0;
    }
  }

  gState.usePos = gState.overridePos;
  gState.useMatrix = gState.overrideDir;
}
