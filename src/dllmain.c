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

#include "internal.h"
#include "ui/ui.h"

HMODULE hModuleDll;

BOOL APIENTRY DllMain(
  HMODULE hModule,
  DWORD dwReason,
  LPVOID lpReserved
) {
  if (dwReason == DLL_PROCESS_ATTACH) {
    hModuleDll = hModule;
    QueryPerformanceFrequency((LARGE_INTEGER *)&gGui.performFreq);
  }

  return TRUE;
}
