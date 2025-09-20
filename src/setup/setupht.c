#ifdef USE_HTML

#include "includes/htmod.h"
#include "ui/ui.h"
#include "internal.h"
#include "setup/setupapi.h"

static void toggleMenu(const HTKeyEvent *event) {
  if (event->flags == HTKeyEventFlags_Down)
    gGui.isOpen = !gGui.isOpen;
}

HTStatus HTMLAPI HTModOnInit(
  void *reserved
) {
  HTHandle hKey = HTHotkeyRegister(
    hModuleDll,
    "Show menu",
    HTKey_F1);
  HTHotkeyListen(
    hKey,
    toggleMenu);
  initAllHooks();
  createAllHooks();

  return HT_SUCCESS;
}

void HTMLAPI HTModRenderGui(
  f32 timeElapesed,
  void *reserved
) {
  // Draw menus.
  if (gGui.isOpen)
    gui_windowMain();
  else
    gGui.showSettings = 0;
  if (gGui.showSettings)
    gui_windowSettings();

  // Handle keyboard input.
  if (gState.enable)
    gui_handleInput();
}

i08 setupAll() {
  return 1;
}

#endif
