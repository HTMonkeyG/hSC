#include <math.h>

#include "internal.h"
#include "ui/ui.h"

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

  if (gState.majorMode == MM_STATIC)
    preupdateStatic(this);
  else if (gState.majorMode == MM_DYNAMIC)
    preupdateDynamic(this);
  else if (gState.majorMode == MM_ANIMATION)
    preupdateAnim(this);
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
