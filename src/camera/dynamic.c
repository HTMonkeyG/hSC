#include "ui/ui.h"
#include "fpv/fpv.h"
#include "fpv/elytra.h"
#include "internal.h"

void hscPreupdateDynamic(MainCamera *this) {
  v4f deltaRot = {0}
    , mouseDelta;

  if (gState.resetPosFlag) {
    gState.resetPosFlag = 0;
    (void)fpvElytra_init(this->context1.cameraPos, V4FZERO, FPVRST_POS);
    fpvElytra_enableRoll(1);
    fpvElytra_enableSmooth(1);
    return;
  }

  mouseDelta = hscGetFacingDeltaRad();
  deltaRot.x = -mouseDelta.x;
  deltaRot.y = mouseDelta.y;
  deltaRot.z = gState.facingInput.z * gGui.timeElapsedSecond;

  // Update elytra.
  (void)fpvElytra_update(gState.movementInput, deltaRot, gGui.timeElapsedSecond);

  //gState.rot = gElytra.rot;
  //gState.pos = gElytra.pos;

  //eulerToRotationXYZ(gState.rot, gState.mat);
  //gState.mat[3] = gState.pos;
  memcpy(gState.mat, &gElytra.matrix, 4 * sizeof(v4f));

  // Override matrix and position.
  gState.usePos = gState.useMatrix = 1;
}
