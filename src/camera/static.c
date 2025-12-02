#include "hsc.h"

#define COLLIDE_RADIUS 0.12f
#define COLLIDE_EPSILON 0.005f
#define COLLIDE_ITER 5

/**
 * Freecam collision check function.
 * Calls CollisionGeoBarn::SphereCast() to do the detection.
 */
static i08 freecamCheckCollision(
  v4f origin,
  v4f *delta
) {
  i08 result = 0;
  i32 iter = 0;
  v4f vec = *delta;
  RaycastResult ir;

  if (v4flen(vec) < COLLIDE_EPSILON)
    vec = v4fscale(v4fnormalize(vec), COLLIDE_EPSILON);

  while (CollisionGeoBarn_sphereCast(
    origin,
    v4fadd(origin, vec),
    COLLIDE_RADIUS,
    NULL,
    &ir
  )) {
    if (iter >= COLLIDE_ITER || v4flen(vec) < COLLIDE_EPSILON) {
      // Directly break the loop if the displacement is small enough, or it
      // still collides after enough checks.
      vec = V4FZERO;
      break;
    }

    // Subtract the projection of the displacement vector onto the normal
    // vector to obtain the tangential component of the displacement vector.
    // Use the tangential vector to iterate the calculation 3 times, to avoid
    // corruptions at the intersection of two faces.
    vec = v4fsub(vec, v4fprojection(vec, ir.normal));

    iter++;
    result = 1;
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
      if (gOptions.control.swapRollYaw) {
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

      if (gContext.freecamCollision)
        // Check collision.
        freecamCheckCollision(gCamera.pos, &delta);

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
