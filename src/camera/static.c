#include "ui/ui.h"
#include "internal.h"

static const v4f AABB_SIZE = {0.1f, 0.1f, 0.1f, 0.1f};

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

void preupdateStatic(MainCamera *this) {
  // This function does:
  // overrideDir
  //   -> Calculate local matrix from rotation inputs.
  //   -> Apply delta rotation from mouse inputs to local matrix.
  //   -> Calculate rotation from local matrix and copy to gui.
  //   -> Copy local matrix to the game.
  // !overrideDir
  //   -> Calc rotation from the game.
  // overridePos
  //   -> Calculate delta position from local matrix (foward vector).
  //   -> Apply delta position from keyboard inputs to local position.
  //   -> Copy local position to the game.
  // !overridePos
  //   -> Copy local position from the game.
  //
  v4f deltaRot = {0}
    , mouseDelta, delta;
  m44 matDelta = {0};
  AABB_t aabb;

  if (gState.overrideDir) {
    mouseDelta = gui_getFacingDeltaRad();

    // Convert facing directions to radians.
    gState.rot = v4fscale(gState.rotDeg, PI_F / 180.0f);
    // Copy the rotation matrix to gState. We must provide the matrix to the
    // gui.
    eulerToRotationXYZ(gState.rot, gState.mat);

    if (!gState.freecamLockRotation) {
      // Calculate rotation delta from hooked data.
      deltaRot.z = gState.facingInput.z
        * gState.freecamRotateSpeed
        * gGui.timeElapsedSecond;
      deltaRot.x = -mouseDelta.x;
      deltaRot.y = mouseDelta.y;
      // Swap yaw and roll inputs.
      if (gOptions.freecam.swapRollYaw) {
        f32 t = deltaRot.z;
        deltaRot.z = deltaRot.x;
        deltaRot.x = t;
      }

      // Calculate rotation matrix based on last frame.
      eulerToRotationXYZ(deltaRot, (v4f *)&matDelta);
      matDelta.row4 = v4fnew(0, 0, 0, 1);
      // Apply rotation delta to local matrix.
      m44mul((m44 *)gState.mat, &matDelta, (m44 *)gState.mat);

      // Calculate rotation from local matrix and show on gui.
      (void)rotationToEulerXYZ(&gState.rot, (v4f *)gState.mat);
      gState.rotDeg = v4fscale(gState.rot, 180.0f / PI_F);
    }
  } else {
    // Sync original facing directions to overlay. We assume that the rotation
    // matrix is in the order of yaw-pitch-roll, so we can apply the following
    // codes.
    (void)rotationToEulerXYZ(&gState.rot, (v4f *)&this->context1.mat);
    gState.rotDeg = v4fscale(gState.rot, 180.0f / PI_F);
    // Copy game rotation matrix to local matrix.
    memcpy((void *)gState.mat, (void *)&this->context1.mat, sizeof(m44));
  }

  if (gState.overridePos && !gState.resetPosFlag) {
    if (!gState.freecamLockPosition) {
      // Calculate the movement direction vector.
      // Foward (gState.mat[1] towards backward, so we need to invert it).
      delta = v4fscale(gState.mat[2], -gState.movementInput.z);
      // Left (gState.mat[0] towards right, so we need to invert it).
      delta = v4fadd(delta, v4fscale(gState.mat[0], -gState.movementInput.x));
      // Up.
      delta = v4fadd(delta, v4fscale(gState.mat[1], gState.movementInput.y));
      // Normalize the vector.
      delta = v4fnormalize(delta);

      // Multiply by speed.
      delta = v4fscale(delta, gState.freecamSpeed * gGui.timeElapsedSecond);

      if (gState.freecamCollision) {
        // Build AABB and check collision.
        aabb_fromCenter(&aabb, gState.pos, AABB_SIZE);
        freecamCheckCollision(&aabb, &delta);
      }

      gState.pos = v4fadd(gState.pos, delta);
    }
  } else {
    if (gState.resetPosFlag)
      gState.resetPosFlag = 0;
    // Copy the camera pos from the game.
    gState.pos = this->context1.cameraPos;
  }

  // Copy position vector to local matrix, the vector is copied to the game in
  // updateCameraMain().
  gState.mat[3] = gState.pos;

  gState.usePos = gState.overridePos;
  gState.useMatrix = gState.overrideDir;
}
