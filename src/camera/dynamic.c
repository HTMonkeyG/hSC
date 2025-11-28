#include "fpv/fpv.h"
#include "fpv/elytra.h"
#include "internal.h"

void hscPreupdateDynamic(
  MainCamera *this
) {
  v4f deltaRot = {0}
    , mouseDelta;

  if (gContext.resetPosFlag) {
    gContext.resetPosFlag = 0;
    (void)hscFpvElytraInit(
      this->context1.cameraPos,
      V4FZERO,
      FPVRST_POS);
    hscFpvElytraEnableRoll(1);
    //hscFpvElytraEnableSmooth(1);
    return;
  }

  mouseDelta = hscGetFacingDeltaRad();
  deltaRot.x = -mouseDelta.x;
  deltaRot.y = mouseDelta.y;
  deltaRot.z = gContext.facingInput.z * gTimeElapsed;

  // Update elytra.
  (void)hscFpvElytraUpdate(
    gContext.movementInput,
    deltaRot,
    gTimeElapsed);

  gCamera.matrix = gElytra.matrix;
  gContext.pos = gCamera.pos;

  // Override matrix and position.
  gCameraIntent.pos = HscIndentAxis_All;
  gCameraIntent.rot = 1;
}
