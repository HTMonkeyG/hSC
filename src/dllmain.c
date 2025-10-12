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

BOOL APIENTRY DllMain(
  HMODULE hModule,
  DWORD dwReason,
  LPVOID lpReserved
) {
  if (dwReason == DLL_PROCESS_ATTACH) {
    gameBaseAddr = (LPVOID)GetModuleHandleA("Sky.exe");

    if (!gameBaseAddr)
      // Not the correct game process.
      return TRUE;

    hModuleDll = hModule;
    QueryPerformanceFrequency((LARGE_INTEGER *)&gGui.performFreq);
    MH_Initialize();
  } else if (dwReason == DLL_PROCESS_DETACH) {
    LOGI("hSC detached.\n");

    // The cleanup procedure is removed. Just let the OS to take over it.
    MH_DisableHook(MH_ALL_HOOKS);
    MH_Uninitialize();
  }

  return TRUE;
}
