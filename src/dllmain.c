// ----------------------------------------------------------------------------
// hSC (hook SkyCamera) Plugin by HTMonkeyG.
// A camera modification plugin for Sky:CotL.
//
// This repository is published under MIT License.
// <https://www.github.com/HTMonkeyG/hSC>.
// ----------------------------------------------------------------------------

#include <windows.h>
#include <stdio.h>
#include <math.h>
#include "includes/htmod.h"
#include "MinHook.h"

#include "internal.h"
#include "ui/ui.h"
#include "log.h"

static LPVOID baseAddr;

HMODULE hModuleDll;

#ifndef USE_HTML

static DWORD WINAPI onAttach(LPVOID lpParam) {
  HMODULE hModule = (HMODULE)lpParam;
  i32 s;
  DWORD le;

  // Wait for the dx12 to be loaded.
  s = gui_waitForDll(&le);
  if (!s) {
    LOGEF("dxgi.dll or d3d12.dll load timed out: %d\n", le);
    return 0;
  }

  // Initialize gui.
  if (!gui_init(hModule)) {
    LOGEF("Gui init failed.\n");
    return 0;
  }
  // We'll wait for the gui.
  if (!gui_waitForInit()) {
    LOGEF("Gui init timed out.\n");
    return 0;
  }
  LOGI("gui_init() successed.\n");

  // Initialize functions. Sleep for a while in order to wait for the game to
  // completely decrypt the instructions.
  Sleep(2500);
  initAllHooks();
  createAllHooks();

  return 0;
}

#endif

BOOL APIENTRY DllMain(
  HMODULE hModule,
  DWORD dwReason,
  LPVOID lpReserved
) {
  DWORD threadId = 0;

  if (dwReason == DLL_PROCESS_ATTACH) {
    baseAddr = (LPVOID)GetModuleHandleA("Sky.exe");

    if (!baseAddr)
      // Not the correct game process.
      return TRUE;

    hModuleDll = hModule;

#ifndef USE_HTML
    recreateConsole();

    LOGI("hSC injected.\n");
    LOGI("(Sky.exe + 0x0): 0x%p\n", baseAddr);

    MH_Initialize();

    if (!CreateThread(
      NULL, 0, onAttach, (LPVOID)hModule, 0, &threadId
    )) {
      LOGEF("Create subthread failed.\n");
      return TRUE;
    }
    LOGI("CreateThread(): 0x%lX\n", threadId);
#else
    MH_Initialize();
#endif
  } else if (dwReason == DLL_PROCESS_DETACH) {
    LOGI("hSC detached.\n");

    // The cleanup procedure is removed. Just let the OS to take over it.
    MH_DisableHook(MH_ALL_HOOKS);
    MH_Uninitialize();
  }

  return TRUE;
}
