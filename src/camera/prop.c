#include "internal.h"

// ----------------------------------------------------------------------------
// [SECTION] Camera prop update functions.
// ----------------------------------------------------------------------------

static inline void overrideOrCopy(
  i08 condition,
  f32 *dest1,
  f32 *dest2,
  f32 *src
) {
  if (condition)
    *dest1 = *dest2 = *src;
  else
    *src = *dest1;
}

static void updatePropStatic(SkyCameraProp *this) {
  // We don't override camera pos in SkyCameraProp to prevent the camera from
  // automatically turning off due to being too far away.
  if (gContext.overrideDir && gContext.cameraMode == HscCameraMode_Selfie) {
    // Override camera rotations.
    this->rotateXAnim = this->rotateX = -gContext.rot.x;
    this->rotateYAnim = this->rotateY = -gContext.rot.y;
    this->rotateSpeedX = this->rotateSpeedY = 0;
  }

  // Override or copy camera params.
  overrideOrCopy(
    gContext.overrideScale,
    &this->scaleAnim,
    &this->scale,
    &gContext.scale);
  overrideOrCopy(
    gContext.overrideFocus,
    &this->focusAnim,
    &this->focus,
    &gContext.focus);
  overrideOrCopy(
    gContext.overrideBrightness,
    &this->brightnessAnim,
    &this->brightness,
    &gContext.brightness);
}

static void updatePropDynamic(
  SkyCameraProp *this
) {
  ;
}

static void updatePropAnim(
  SkyCameraProp *this
) {
  ;
}

/**
 * Calculate the rotation matrix and camera pos with gui data, and save
 * caculated data to gContext.
 */
void hscUpdatePropMain(SkyCameraProp *this) {
  if (!gContext.enable)
    return;

  if (gContext.majorMode == HscMajorMode_Static)
    updatePropStatic(this);
  else if (gContext.majorMode == HscMajorMode_Dynamic)
    updatePropDynamic(this);
  else if (gContext.majorMode == HscMajorMode_Animation)
    updatePropAnim(this);
}
