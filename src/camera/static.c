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
      ((PFN_World_interactionTest)gTramp.fn_Level_interactionTest)(
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
      vec = v4fsub(vec, v4fprojection(vec, ir.normal));
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

void hscPreupdateStatic(
  MainCamera *this
) {
  // This function does:
  // overrideDir
  //   -> Calculate local matrix from rotation inputs.
  //   -> Apply delta rotation from mouse inputs to local matrix.
  //   -> Calculate rotation from local matrix and copy to gui.
  //   -> Copy local matrix to the game.
  // !overrideDir
  //   -> Calculate rotation from the game.
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

  if (gContext.overrideDir) {
    mouseDelta = hscGetFacingDeltaRad();

    // Convert facing directions to radians.
    gContext.rot = v4fscale(gContext.rotDeg, PI_F / 180.0f);
    // Copy the rotation matrix to gCamera. eulerToRotationXYZ() only changes
    // row1 ~ row3 of the matrix.
    eulerToRotationXYZ(gContext.rot, (v4f *)&gCamera.matrix);

    if (!gContext.freecamLockRotation) {
      // Calculate rotation delta from hooked data.
      deltaRot.z = gContext.facingInput.z
        * gContext.freecamRotateSpeed
        * gTimeElapsed;
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
      // | ?, ?, ?, 0 |   | ?, ?, ?, 0 |
      // | ?, ?, ?, 0 | * | ?, ?, ?, 0 |
      // | ?, ?, ?, 0 |   | ?, ?, ?, 0 |
      // | 0, 0, 0, 1 |   | #, #, #, 1 |
      //    matDelta      gCamera.matrix
      m44mul(&gCamera.matrix, &matDelta, &gCamera.matrix);

      // Calculate rotation from local matrix and show on gui.
      (void)rotationToEulerXYZ(&gContext.rot, (v4f *)&gCamera.matrix);
      gContext.rotDeg = v4fscale(gContext.rot, 180.0f / PI_F);
    }
  } else {
    // Sync original facing directions to overlay. We assume that the rotation
    // matrix is in the order of yaw-pitch-roll, so we can apply the following
    // codes.
    (void)rotationToEulerXYZ(&gContext.rot, (v4f *)&this->context1.mat);
    gContext.rotDeg = v4fscale(gContext.rot, 180.0f / PI_F);
    // Copy game rotation matrix to local matrix.
    v4f pos = gCamera.pos;
    gCamera.matrix = this->context1.mat;
    gCamera.pos = pos;
  }

  hscValidateSnapshot(&gCamera);

  if (gContext.overridePos && !gContext.resetPosFlag) {
    gCamera.pos = gContext.pos;

    if (!gContext.freecamLockPosition) {
      // Calculate the movement direction vector.
      // Foward (gCamera.matrix.row3 towards backward, so we need to invert it).
      delta = v4fscale(gCamera.backward, -gContext.movementInput.z);
      // Left (gCamera.matrix.row1 towards right, so we need to invert it).
      delta = v4fadd(delta, v4fscale(gCamera.left, -gContext.movementInput.x));
      // Up.
      delta = v4fadd(delta, v4fscale(gCamera.up, gContext.movementInput.y));
      // Normalize the vector.
      delta = v4fnormalize(delta);

      // Multiply by speed.
      delta = v4fscale(delta, gContext.freecamSpeed * gTimeElapsed);

      if (gContext.freecamCollision) {
        // Build AABB and check collision.
        aabb_fromCenter(&aabb, gCamera.pos, AABB_SIZE);
        freecamCheckCollision(&aabb, &delta);
      }

      gCamera.pos = v4fadd(gCamera.pos, delta);
    }
  } else {
    if (gContext.resetPosFlag)
      gContext.resetPosFlag = 0;
    // Copy the camera pos from the game.
    gCamera.pos = this->context1.cameraPos;
  }

  // Copy position vector to local matrix, the vector is copied to the game in
  // hscUpdateCameraMain().
  gContext.pos = gCamera.pos;
  hscValidateSnapshot(&gCamera);

  gCameraIntent.pos = gContext.overridePos
    ? HscIndentAxis_All
    : HscIndentAxis_None;
  gCameraIntent.rot = gContext.overrideDir;
}
