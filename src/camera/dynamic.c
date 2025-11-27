#include "ui/ui.h"
#include "fpv/fpv.h"
#include "fpv/elytra.h"
#include "internal.h"

void hscPreupdateDynamic(
  MainCamera *this
) {
  v4f deltaRot = {0}
    , mouseDelta;

  if (gState.resetPosFlag) {
    gState.resetPosFlag = 0;
    (void)hscFpvElytraInit(
      this->context1.cameraPos,
      V4FZERO,
      FPVRST_POS);
    hscFpvElytraEnableRoll(1);
    hscFpvElytraEnableSmooth(1);
    return;
  }

  mouseDelta = hscGetFacingDeltaRad();
  deltaRot.x = -mouseDelta.x;
  deltaRot.y = mouseDelta.y;
  deltaRot.z = gState.facingInput.z * gGui.timeElapsedSecond;

  // Update elytra.
  (void)hscFpvElytraUpdate(
    gState.movementInput,
    deltaRot,
    gGui.timeElapsedSecond);

  gCamera.matrix = gElytra.matrix;
  gState.pos = gCamera.pos;

  // Override matrix and position.
  gCameraIntent.pos = HscIndentAxis_All;
  gCameraIntent.rot = 1;
}
