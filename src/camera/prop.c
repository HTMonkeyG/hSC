#include "ui/ui.h"
#include "internal.h"

#define OVERRIDE_2(cond, v1, v2) ((cond) ? ((v1) = (v2)) : ((v2) = (v1)))
#define OVERRIDE_3(cond, v11, v12, v2) ((cond) ? ((v11) = ((v12) = (v2))) : ((v2) = (v11)))

// ----------------------------------------------------------------------------
// [SECTION] Camera prop update functions.
// ----------------------------------------------------------------------------

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

static void updatePropDynamic(SkyCameraProp *this) {
  ;
}

static void updatePropAnim(SkyCameraProp *this) {
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
