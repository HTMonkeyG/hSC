#include "imgui.h"

#include "mth/macros.h"
#include "ui/ui.h"

v4f gMouseDeltaPx = {0};
KeyBindings_t gBindedKeys;

static void inputStaticMode() {
  v4f r = v4fnew(0.0f, 0.0f, 0.0f, 0.0f)
    , s = v4fnew(0.0f, 0.0f, 0.0f, 0.0f);

  // Movement.
  if (HTHotkeyPressed(gBindedKeys.foward))
    r.z += 1.0f;
  if (HTHotkeyPressed(gBindedKeys.backward))
    r.z -= 1.0f;
  if (HTHotkeyPressed(gBindedKeys.left))
    r.x += 1.0f;
  if (HTHotkeyPressed(gBindedKeys.right))
    r.x -= 1.0f;

  // Up and down.
  if (HTHotkeyPressed(gBindedKeys.up))
    r.y += 1.0f;
  if (HTHotkeyPressed(gBindedKeys.down))
    r.y -= 1.0f;

  // Roll.
  if (HTHotkeyPressed(gBindedKeys.rollLeft))
    s.z += 1.0f;
  if (HTHotkeyPressed(gBindedKeys.rollRight))
    s.z -= 1.0f;

  gState.movementInput = r;
  gState.facingInput = s;
}

static void inputDynamicMode() {
  v4f r = v4fnew(0.0f, 0.0f, 0.0f, 0.0f)
    , s = v4fnew(0.0f, 0.0f, 0.0f, 0.0f);

  // Movement.
  if (HTHotkeyPressed(gBindedKeys.foward))
    r.z += 1.0f;
  if (HTHotkeyPressed(gBindedKeys.backward))
    r.z -= 1.0f;
  if (HTHotkeyPressed(gBindedKeys.left))
    r.x += 1.0f;
  if (HTHotkeyPressed(gBindedKeys.right))
    r.x -= 1.0f;

  // Up and down.
  if (HTHotkeyPressed(gBindedKeys.up))
    r.y += 1.0f;
  if (HTHotkeyPressed(gBindedKeys.down))
    r.y -= 1.0f;

  gState.movementInput = r;
  gState.facingInput = s;
}

void hscInputHandler() {
  if (gState.majorMode == MM_STATIC)
    inputStaticMode();
  if (gState.majorMode == MM_DYNAMIC)
    inputDynamicMode();
}

v4f hscGetFacingDeltaRad() {
  v4f result = gMouseDeltaPx;

  // About 16000px per 360 degree when the sensitivity is 1.
  result = v4fscale(
    result,
    2.0f * PI_F / 16384.0f / 4.0f * gOptions.general.mouseSensitivity);
  result.y *= gOptions.general.verticalSenseScale;

  return result;
}
