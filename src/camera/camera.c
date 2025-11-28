#include <math.h>

#include "internal.h"

HscSnapshot gCamera;
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
  LARGE_INTEGER qpc;
  i64 inteval;

  // Calculate time elapsed since last frame.
  if (!gLastFrame.QuadPart)
    QueryPerformanceCounter(&gLastFrame);
  else {
    QueryPerformanceCounter(&qpc);
    inteval = qpc.QuadPart - gLastFrame.QuadPart;
    gLastFrame = qpc;
    gTimeElapsed = (f32)inteval / (f32)gPerformFreq.QuadPart;
  }

  if (!gContext.enable)
    return;

  gCameraIntent.pos = gCameraIntent.rot = 0;

  if (gContext.majorMode == HscMajorMode_Static)
    hscPreupdateStatic(this);
  else if (gContext.majorMode == HscMajorMode_Dynamic)
    hscPreupdateDynamic(this);
  else if (gContext.majorMode == HscMajorMode_Animation)
    hscPreupdateAnim(this);
}

/**
 * Copy calculated rotation matrix and position vector.
 */
void hscUpdateCameraMain(
  MainCamera *this
) {
  if (!gContext.enable)
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
