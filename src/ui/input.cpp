#include "imgui.h"

#include "mth/macros.h"
#include "internal.h"

v4f gMouseDeltaPx = {0};
HscKeyBindings gBindedKeys;

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

  gContext.movementInput = r;
  gContext.facingInput = s;
}

static void inputDynamicMode() {
  v4f r = v4fnew(0.0f, 0.0f, 0.0f, 0.0f)
    , s = v4fnew(0.0f, 0.0f, 0.0f, 0.0f);

  // Movement.
  if (HTHotkeyPressed(gBindedKeys.foward))
    r.z += 1.0f;
  if (HTHotkeyPressed(gBindedKeys.backward))
    r.z -= 1.0f;

  // Roll.
  if (HTHotkeyPressed(gBindedKeys.left))
    s.z += 1.0f;
  if (HTHotkeyPressed(gBindedKeys.right))
    s.z -= 1.0f;

  // Up and down.
  if (HTHotkeyPressed(gBindedKeys.up))
    r.y += 1.0f;
  if (HTHotkeyPressed(gBindedKeys.down))
    r.y -= 1.0f;

  gContext.movementInput = r;
  gContext.facingInput = s;
}

void hscInputHandler() {
  if (gContext.majorMode == HscMajorMode_Static)
    inputStaticMode();
  if (gContext.majorMode == HscMajorMode_Dynamic)
    inputDynamicMode();
}

static void hscKeyEventCallbackStatic(
  HTKeyEvent *event
) {
  if (
    event->hKey == gBindedKeys.foward
    || event->hKey == gBindedKeys.backward
    || event->hKey == gBindedKeys.left
    || event->hKey == gBindedKeys.right
    || event->hKey == gBindedKeys.up
    || event->hKey == gBindedKeys.down
  ) {
    if (
      event->flags == HTKeyEventFlags_Down
      && gContext.enable
      && !gContext.freecamLockPosition
      && gContext.overridePos
    )
      // Like elder hSC versions, we only block keyDown messages.
      event->preventFlags |= HTKeyEventPreventFlags_Game;
  } else if (
    event->hKey == gBindedKeys.rollLeft
    || event->hKey == gBindedKeys.rollRight
  ) {
    if (
      event->flags == HTKeyEventFlags_Down
      && gContext.enable
      && !gContext.freecamLockRotation
      && gContext.overrideDir
    )
      // Like elder hSC versions, we only block keyDown messages.
      event->preventFlags |= HTKeyEventPreventFlags_Game;
  }
}

static void hscKeyEventCallbackDynamic(
  HTKeyEvent *event
) {
  if (
    event->hKey == gBindedKeys.foward
    || event->hKey == gBindedKeys.backward
    || event->hKey == gBindedKeys.left
    || event->hKey == gBindedKeys.right
    || event->hKey == gBindedKeys.up
    || event->hKey == gBindedKeys.down
    || event->hKey == gBindedKeys.rollLeft
    || event->hKey == gBindedKeys.rollRight
  ) {
    if (
      event->flags == HTKeyEventFlags_Down
      && gContext.enable
    )
      // Like elder hSC versions, we only block keyDown messages.
      event->preventFlags |= HTKeyEventPreventFlags_Game;
  }
}

void hscKeyEventCallbackMain(
  HTKeyEvent *event
) {
  if (event->hKey == gBindedKeys.menu) {
    if (event->flags == HTKeyEventFlags_Down)
      // Toggle the menu status when the key is pressed.
      gContext.isMenuShown = !gContext.isMenuShown;
    return;
  }

  if (gContext.majorMode == HscMajorMode_Static)
    hscKeyEventCallbackStatic(event);
  if (gContext.majorMode == HscMajorMode_Dynamic)
    hscKeyEventCallbackDynamic(event);
}

v4f hscGetFacingDeltaRad() {
  v4f result = gMouseDeltaPx;

  // About 16000px per 360 degree when the sensitivity is 1.
  result = v4fscale(
    result,
    2.0f * PI_F / 16384.0f / 4.0f * gOptions.control.mouseSensitivity);
  result.y *= gOptions.control.verticalSenseScale;

  return result;
}
