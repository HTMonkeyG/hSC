#include "ui/ui.h"
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
  if (gState.overrideDir && gState.cameraMode == CM_FRONT) {
    // Override camera rotations.
    this->rotateXAnim = this->rotateX = -gState.rot.x;
    this->rotateYAnim = this->rotateY = -gState.rot.y;
    this->rotateSpeedX = this->rotateSpeedY = 0;
  }

  // Override or copy camera params.
  overrideOrCopy(
    gState.overrideScale,
    &this->scaleAnim,
    &this->scale,
    &gState.scale);
  overrideOrCopy(
    gState.overrideFocus,
    &this->focusAnim,
    &this->focus,
    &gState.focus);
  overrideOrCopy(
    gState.overrideBrightness,
    &this->brightnessAnim,
    &this->brightness,
    &gState.brightness);
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
 * caculated data to gState.
 */
void hscUpdatePropMain(SkyCameraProp *this) {
  if (!gState.enable)
    return;

  if (gState.majorMode == MM_STATIC)
    updatePropStatic(this);
  else if (gState.majorMode == MM_DYNAMIC)
    updatePropDynamic(this);
  else if (gState.majorMode == MM_ANIMATION)
    updatePropAnim(this);
}
