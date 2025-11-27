#include <math.h>

#include "internal.h"
#include "ui/ui.h"

// Global context of the plugin.
HscContext gContext;

// The camera status to be updated to the current camera object of the game.
HscSnapshot gCamera;
// The avaliable fields in gCamera.
HscSnapshotIntent gCameraIntent;

/**
 * Get the time elapsed since last frame, and calculate rotation matrix and
 * posision vector.
 * 
 * NOTE: This function must ONLY execute ONCE in every frame.
 */
void hscPreupdateCameraMain(
  MainCamera *this
) {
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

  gCameraIntent.pos = gCameraIntent.rot = 0;

  if (gState.majorMode == MM_STATIC)
    hscPreupdateStatic(this);
  else if (gState.majorMode == MM_DYNAMIC)
    hscPreupdateDynamic(this);
  else if (gState.majorMode == MM_ANIMATION)
    hscPreupdateAnim(this);
}

/**
 * Copy calculated rotation matrix and position vector.
 */
void hscUpdateCameraMain(
  MainCamera *this
) {
  if (!gState.enable)
    return;

  hscValidateSnapshot(&gCamera);

  if (gCameraIntent.rot) {
    memcpy(
      &this->context1.mat,
      &gCamera.matrix,
      3 * sizeof(v4f));
  }
  if (gCameraIntent.pos) {
    this->context1.cameraPos = v4fblend(
      this->context1.cameraPos,
      gCamera.pos,
      gCameraIntent.pos);
  }
}
