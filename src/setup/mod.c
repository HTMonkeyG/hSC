#include "includes/htmod.h"
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
  "8 Roll right",
  "9 Reset pos",
  "10 Start/Stop"
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
  HTKey_E,
  HTKey_None,
  HTKey_None
};

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
      hscKeyEventCallbackMain);
  }

  // Create all hooks.
  hscInitAllHooks();
  hscCreateAllHooks();

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
  if (gContext.isMenuShown)
    hscUIWindowMain();

  // Handle keyboard input.
  if (gContext.enable)
    hscInputHandler();
}
