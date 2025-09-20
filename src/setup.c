#include <windows.h>

#include "internal.h"
#include "log.h"
#include "sigcodes.h"

i08 setupFuncWithSig(SetupFunctions_t *functions) {
  i08 r = 1;
  i32 length;
  void *p;
  const Signature_t *sig;

  LOGI("Scaning functions...\n");

  if (!functions)
    return 0;

  length = sizeof(funcSig) / sizeof(Signature_t *);
  for (i32 i = 0; i < length; i++) {
    sig = funcSig[i];
    if (!sig)
      continue;

    if (sig->indirect)
      p = sigScanE8("Sky.exe", sig->sig, sig->offset);
    else
      p = sigScan("Sky.exe", sig->sig, sig->offset);

    functions->functions[i] = p;

    if (p)
      LOGI("Found %s: 0x%p\n", sig->name, p);
    else {
      LOGE("Scan %s failed!\n", sig->name);
      r = 0;
    }
  }
  
  return r;
}

static inline void wcstoutf8(const wchar_t *wcs, char *utf8, i32 max) {
  char *buf;
  i32 size;

  if (!wcs || !utf8)
    return;

  *utf8 = 0;
  size = WideCharToMultiByte(CP_UTF8, 0, wcs, -1, NULL, 0, NULL, NULL);
  buf = (char *)malloc(size);
  if (!buf)
    return;
  WideCharToMultiByte(CP_UTF8, 0, wcs, -1, buf, size, NULL, NULL);
  strcpy_s(utf8, max, buf);
  free(buf);
  utf8[max - 1] = 0;
}

i08 setupPaths(HMODULE hModule, wchar_t *prefPath, char *guiIniPath) {
  wchar_t dllPath[MAX_PATH]
    , tmpPath[MAX_PATH]
    , *p;
  u32 len;

  len = GetModuleFileNameW(hModule, dllPath, MAX_PATH);
  if (!len || len >= MAX_PATH)
    return 0;
  p = wcsrchr(dllPath, L'\\');
  if (!p)
    return 0;
  *p = 0;

  // History and preference data of hSC.
  wcscpy_s(prefPath, MAX_PATH, dllPath);
  wcscat_s(prefPath, MAX_PATH, L"\\.hsc-data");

  // ImGui uses UTF-8 codepage in paths, so we need the conversion below.
  wcscpy_s(tmpPath, MAX_PATH, dllPath);
  wcscat_s(tmpPath, MAX_PATH, L"\\.hsc-gui.ini");
  wcstoutf8(tmpPath, guiIniPath, MAX_PATH);

  return 1;
}
