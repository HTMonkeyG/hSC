#ifdef USE_HTML

#include "includes/htmod.h"
#include "ui/ui.h"
#include "internal.h"

// HTML uses key name to sort key bindings, so we add a number to make the key
// binding list a little bit more tidy.
static const char *KEY_NAMES[] = {
  "0 Show menu",
  "1 Foward",
  "2 Back",
  "3 Left",
  "4 Right",
  "5 Up",
  "6 Down",
  "7 Roll left",
  "8 Roll right"
};
static const HTKeyCode DEFAULT_KEYS[] = {
  HTKey_F1,
  HTKey_W,
  HTKey_S,
  HTKey_A,
  HTKey_D,
  HTKey_Space,
  HTKey_LeftShift,
  HTKey_Q,
  HTKey_E
};

static void keyEventCallbackGeneral(HTKeyEvent *event) {
  if (event->hKey == gBindedKeys.menu) {
    if (event->flags == HTKeyEventFlags_Down)
      // Toggle the menu status when the key is pressed.
      gGui.isOpen = !gGui.isOpen;
  } else if (
    event->hKey == gBindedKeys.foward
    || event->hKey == gBindedKeys.backward
    || event->hKey == gBindedKeys.left
    || event->hKey == gBindedKeys.right
    || event->hKey == gBindedKeys.up
    || event->hKey == gBindedKeys.down
  ) {
    if (
      event->flags == HTKeyEventFlags_Down
      && gState.enable
      && !gState.freecamLockPosition
      && gState.overridePos
    )
      // Like elder hSC versions, we only block keyDown messages.
      event->preventFlags |= HTKeyEventPreventFlags_Game;
  } else if (
    event->hKey == gBindedKeys.rollLeft
    || event->hKey == gBindedKeys.rollRight
  ) {
    if (
      event->flags == HTKeyEventFlags_Down
      && gState.enable
      && !gState.freecamLockRotation
      && gState.overrideDir
    )
      // Like elder hSC versions, we only block keyDown messages.
      event->preventFlags |= HTKeyEventPreventFlags_Game;
  }
}

__declspec(dllexport) HTStatus HTMLAPI HTModOnInit(
  void *reserved
) {
  for (i32 i = 0; i < sizeof(KEY_NAMES) / sizeof(KEY_NAMES[0]); i++) {
    gBindedKeys.keys[i] = HTHotkeyRegister(
      hModuleDll,
      KEY_NAMES[i],
      DEFAULT_KEYS[i]);
    HTHotkeyListen(
      gBindedKeys.keys[i],
      keyEventCallbackGeneral);
  }

  // Create all hooks.
  initAllHooks();
  createAllHooks();

  return HT_SUCCESS;
}

__declspec(dllexport) HTStatus HTMLAPI HTModOnEnable(
  void *
) {
  return HT_SUCCESS;
}

__declspec(dllexport) void HTMLAPI HTModRenderGui(
  f32 timeElapesed,
  void *reserved
) {
  // Draw menus.
  if (gGui.isOpen)
    gui_windowMain();

  // Handle keyboard input.
  if (gState.enable)
    gui_handleInput();
}

#endif
