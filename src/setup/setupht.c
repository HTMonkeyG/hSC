#ifdef USE_HTML

#include "includes/htmod.h"
#include "ui/ui.h"
#include "internal.h"
#include "setup/setupapi.h"

static void toggleMenu(const HTKeyEvent *event) {
  if (event->flags == HTKeyEventFlags_Down)
    gGui.isOpen = !gGui.isOpen;
}

// HTML v1.0.1 uses key name to sort key bindings, so we add a number to
// make the key binding list a little bit more tidy.
static const char *KEY_NAMES[] = {
  "0 Show menu",
  "1 Foward",
  "2 Back",
  "3 Left",
  "4 Right",
  "5 Up",
  "6 Down",
  "7 Roll left",
  "8, Roll right"
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

__declspec(dllexport) HTStatus HTMLAPI HTModOnInit(
  void *reserved
) {
  for (i32 i = 0; i < sizeof(KEY_NAMES) / sizeof(KEY_NAMES[0]); i++) {
    gBindedKeys.keys[i] = HTHotkeyRegister(
      hModuleDll,
      KEY_NAMES[i],
      DEFAULT_KEYS[i]);
  }
  HTHotkeyListen(
    gBindedKeys.menu,
    toggleMenu);

  // Create all hooks.
  initAllHooks();
  createAllHooks();

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

i08 setupAll() {
  return 1;
}

#endif
