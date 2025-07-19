#include <math.h>

#include "mth/matrix.h"
#include "setup.h"
#include "fpv/fpv.h"
#include "fpv/elytra.h"
#include "ui/gui.h"
#include "ui/input.h"
#include "mth/aabb.h"
#include "camera.h"
#include "hooked.h"

// ----------------------------------------------------------------------------
// [SECTION] Declarations and definitions.
// ----------------------------------------------------------------------------

// Macros.
#define OVERRIDE_2(cond, v1, v2) ((cond) ? ((v1) = (v2)) : ((v2) = (v1)))
#define OVERRIDE_3(cond, v11, v12, v2) ((cond) ? ((v11) = ((v12) = (v2))) : ((v2) = (v11)))

// Typedefs.
typedef u64 (__fastcall *FnWorld_interactionTest)(
  u64, v4f *, v4f *, float, v4f *, i08 *);

// External variables.
// gTramp trampoline functions defined in hooked.c
extern SetupFunctions_t gTramp;
// gFpv defined in fpv.c
extern FPV_t gFpv;

// ----------------------------------------------------------------------------
// [SECTION] Static helper functions.
// ----------------------------------------------------------------------------

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
}

static inline void rotationToEulerXYZ(v4f *matrix, v4f euler) {

}

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

// ----------------------------------------------------------------------------
// [SECTION] Camera update functions.
// ----------------------------------------------------------------------------

static void updatePropSet(SkyCameraProp *this) {
  if (gState.overrideDir && gState.cameraMode != CM_PLACE) {
    this->rotateXAnim = this->rotateX = (gState.rot.x - 180.0f) / 180.0f * PI_F;
    this->rotateYAnim = this->rotateY = -gState.rot.y / 180.0f * PI_F;
    this->rotateSpeedX = this->rotateSpeedY = 0;
  }

  // Override or sync camera params.
  OVERRIDE_3(
    gState.overrideScale,
    this->scaleAnim,
    this->scale,
    gState.scale);
  OVERRIDE_3(
    gState.overrideFocus,
    this->focusAnim,
    this->focus,
    gState.focus);
  OVERRIDE_3(
    gState.overrideBrightness,
    this->brightnessAnim,
    this->brightness,
    gState.brightness);
}

static void updatePropFreecam(SkyCameraProp *this) {
  ;
}

static void updatePropFPV(SkyCameraProp *this) {
  ;
}

/**
 * Calculate the rotation matrix and camera pos with gui data, and save
 * caculated data to gState.
 */
void updatePropMain(SkyCameraProp *this) {
  if (!gState.enable)
    return;

  if (gState.overrideMode == OM_SET)
    updatePropSet(this);
  else if (gState.overrideMode == OM_FREE)
    updatePropFreecam(this);
  else if (gState.overrideMode == OM_FPV)
    updatePropFPV(this);
}

void preupdateSet(MainCamera *this) {
  v4f *pos, *dir, *gsRot
    , rot;

  dir = &this->context1.mat3;
  pos = &this->context1.cameraPos;
  gsRot = &gState.rot;

  // Override coodinates or sync original camera pos to overlay.
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

  // FIXME: When look direct up, the horizontal direction vector will be a zero
  // vector, so the camera won't move.
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

  // Multiply by speed.
  gState.pos = v4fadd(
    lastPos,
    delta);
  gState.mat[3] = gState.pos;

  // Always override the position.
  gState.usePos = 1;
}

void preupdateFPV(MainCamera *this) {
  v4f deltaRot = {0}
    , mouseDelta;

  if (gState.resetPosFlag) {
    gState.resetPosFlag = 0;
    (void)fpvElytra_init(this->context1.cameraPos, V4FZERO, FPVRST_POS);
    return;
  }

  mouseDelta = gui_getFacingDeltaRad();
  deltaRot.x = -mouseDelta.x;
  deltaRot.y = mouseDelta.y;

  // Update elytra.
  (void)fpvElytra_update(gState.movementInput, deltaRot, gGui.timeElapsedSecond);

  gState.rot = gElytra.rot;
  gState.pos = gElytra.pos;

  eulerToRotationXYZ(gState.rot, gState.mat);
  gState.mat[3] = gState.pos;

  // Override matrix and position.
  gState.usePos = gState.useMatrix = 1;
}

/**
 * Get the time elapsed since last frame, and calculate rotation matrix and
 * posision vector.
 * 
 * NOTE: This function must ONLY execute ONCE in every frame.
 */
void preupdateCameraMain(MainCamera *this) {
  i64 qpc, inteval;

  // Calculate time elapsed since last frame.
  if (!gGui.lastFrameCounter)
    QueryPerformanceCounter((LARGE_INTEGER *)&gGui.lastFrameCounter);
  else {
    QueryPerformanceCounter((LARGE_INTEGER *)&qpc);
    inteval = qpc - gGui.lastFrameCounter;
    gGui.lastFrameCounter = qpc;
    gGui.timeElapsedSecond = (f32)inteval / (f32)gGui.performFreq;
  }

  if (!gState.enable)
    return;

  gState.useMatrix = gState.usePos = 0;

  if (gState.overrideMode == OM_SET)
    preupdateSet(this);
  else if (gState.overrideMode == OM_FREE)
    preupdateFreecam(this);
  else if (gState.overrideMode == OM_FPV)
    preupdateFPV(this);
}

/**
 * Copy calculated rotation matrix and position vector.
 */
void updateCameraMain(MainCamera *this) {
  if (!gState.enable)
    return;

  if (gState.useMatrix) {
    this->context1.mat1 = gState.mat[0];
    this->context1.mat2 = gState.mat[1];
    this->context1.mat3 = gState.mat[2];
  }
  if (gState.usePos) {
    this->context1.cameraPos = gState.mat[3];
    this->context1.cameraPos.w = 1.0f;
  }
}
