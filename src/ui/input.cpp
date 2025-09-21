#include "imgui.h"

#include "mth/macros.h"
#include "ui/ui.h"

v4f gMouseDeltaPx = {0};

static void gui_inputFreecam() {
  v4f r = v4fnew(0.0f, 0.0f, 0.0f, 0.0f)
    , s = v4fnew(0.0f, 0.0f, 0.0f, 0.0f);

  // Movement.
  if (HTHotkeyPressed(gBindedKeys.foward))
    r.z += 1.0f;
  if (HTHotkeyPressed(gBindedKeys.backward))
    r.z -= 1.0f;
  if (HTHotkeyPressed(gBindedKeys.left))
    gState.freecamMode != FC_FULLDIR
      ? r.x += 1.0f
      : s.z += 1.0f;
  if (HTHotkeyPressed(gBindedKeys.right))
    gState.freecamMode != FC_FULLDIR
      ? r.x -= 1.0f
      : s.z -= 1.0f;

  // Up and down.
  if (HTHotkeyPressed(gBindedKeys.up))
    r.y += 1.0f;
  if (HTHotkeyPressed(gBindedKeys.down))
    r.y -= 1.0f;

  gState.movementInput = r;
  gState.facingInput = s;
}

static void gui_inputFPV() {
  v4f r = v4fnew(0.0f, 0.0f, 0.0f, 0.0f)
    , s = v4fnew(0.0f, 0.0f, 0.0f, 0.0f);

  // Movement.
  if (ImGui::IsKeyDown(ImGuiKey_W))
    r.z += 1.0f;
  if (ImGui::IsKeyDown(ImGuiKey_A))
    s.z += 1.0f;
  //if (ImGui::IsKeyDown(ImGuiKey_S))
  //  r.z -= 1.0f;
  if (ImGui::IsKeyDown(ImGuiKey_D))
    s.z -= 1.0f;

  // Up and down.
  if (ImGui::IsKeyDown(ImGuiKey_Space))
    r.y += 1.0f;
  //if (ImGui::IsKeyDown(ImGuiKey_LeftShift))
  //  r.y -= 1.0f;

  gState.movementInput = r;
  gState.facingInput = s;
}

void gui_handleInput() {
  if (gState.majorMode == MM_STATIC)
    gui_inputFreecam();
  if (gState.majorMode == MM_DYNAMIC)
    gui_inputFPV();
}

v4f gui_getFacingDeltaRad() {
  v4f result = gMouseDeltaPx;

  // About 16000px per 360 degree when the sensitivity is 1.
  result = v4fscale(
    result,
    2.0f * PI_F / 16384.0f / 4.0f * gOptions.general.mouseSensitivity);
  result.y *= gOptions.general.verticalSenseScale;

  return result;
}
