#include <Windows.h>
#include <stdio.h>
#include <stdarg.h>
#include <string>
#include <htmodloader/htmodloader.h>
#include "utils/logger.hpp"
#include "utils/loaderUtils.hpp"

// ----------------------------------------------------------------------------
// [SECTION] LowLevel
// ----------------------------------------------------------------------------

// Since HTModLoader does not have bitwise mask, so we consider every mask byte
// that is not 0xFF to be "?".
static TgcString s_BuildHTPattern(
  const u08 *pattern,
  const u08 *masks,
  u32 length
) {
  constexpr cstring kHexChars = "0123456789ABCDEF";
  TgcString result(length * 3, '\0');

  for (u32 i = 0; i < length; i++) {
    if (masks[i] != 0xFF) {
      result[i * 3] = '?';
      result[i * 3 + 1] = ' ';
    } else {
      result[i * 3] = kHexChars[(pattern[i] >> 4) & 0xF];
      result[i * 3 + 1] = kHexChars[pattern[i] & 0xF];
    }
    result[i * 3 + 2] = ' ';
  }

  return result;
}

static HMODULE g_hModule = nullptr;

namespace HTModLoader {

HMODULE GetCurrentModuleHandle() {
  return g_hModule;
}

void SetCurrentModuleHandle(HMODULE hModule) {
  g_hModule = hModule;
}

bool LoadSetting(cstring key, TgcString *result) {
  if (result == nullptr)
    return false;

  HMODULE hModule = HTModLoader::GetCurrentModuleHandle();

  UINT32 cch = 0;
  HTStatus status = HTOptionGetCustom(hModule, key, HTOptionType_String, nullptr, &cch);
  if (status != HT_SUCCESS || cch == 0)
    return false;

  TgcString buffer(static_cast<size_t>(cch), '\0');

  UINT32 written = cch;
  status = HTOptionGetCustom(hModule, key, HTOptionType_String, buffer.data(), &written);
  if (status != HT_SUCCESS)
    return false;

  // `written` may include the terminating null, and the loader is allowed to
  // report a smaller count than cch.
  if (written <= cch)
    buffer.resize(written);
  while (!buffer.empty() && buffer.back() == '\0')
    buffer.pop_back();

  *result = buffer;
  return true;
}

bool LoadSetting(cstring key, bool *result) {
  if (result == nullptr)
    return false;

  HMODULE hModule = HTModLoader::GetCurrentModuleHandle();

  // A Bool option occupies a Windows BOOL (4 bytes), not a C++ bool.
  BOOL buffer = FALSE;
  HTStatus status = HTOptionGetCustom(hModule, key, HTOptionType_Bool, &buffer, nullptr);
  if (status != HT_SUCCESS)
    return false;

  *result = buffer != FALSE;
  return true;
}

bool LoadSetting(cstring key, f64 *result) {
  if (result == nullptr)
    return false;

  HMODULE hModule = HTModLoader::GetCurrentModuleHandle();

  f64 buffer;
  HTStatus status = HTOptionGetCustom(hModule, key, HTOptionType_Double, &buffer, nullptr);
  if (status != HT_SUCCESS)
    return false;

  *result = buffer;
  return true;
}

bool LoadSetting(cstring key, i32 *result) {
  f64 buffer;
  if (!LoadSetting(key, &buffer))
    return false;
  *result = (i32)buffer;
  return true;
}

bool SaveSetting(cstring key, const TgcString &value) {
  HMODULE hModule = HTModLoader::GetCurrentModuleHandle();
  return HTOptionSetCustom(hModule, key, HTOptionType_String, value.c_str()) == HT_SUCCESS
    ? true
    : false;
}

bool SaveSetting(cstring key, bool value) {
  HMODULE hModule = HTModLoader::GetCurrentModuleHandle();
  BOOL buffer = value ? TRUE : FALSE;
  return HTOptionSetCustom(hModule, key, HTOptionType_Bool, &buffer) == HT_SUCCESS
    ? true
    : false;
}

bool SaveSetting(cstring key, f64 value) {
  HMODULE hModule = HTModLoader::GetCurrentModuleHandle();
  return HTOptionSetCustom(hModule, key, HTOptionType_Double, &value) == HT_SUCCESS
    ? true
    : false;
}

bool SaveSetting(cstring key, i32 value) {
  f64 buffer = value;
  HMODULE hModule = HTModLoader::GetCurrentModuleHandle();
  return HTOptionSetCustom(hModule, key, HTOptionType_Double, &buffer) == HT_SUCCESS
    ? true
    : false;
}

uintptr_t ScanPattern(
  uintptr_t module,
  const u08 *pattern,
  const u08 *masks,
  u32 length
) {
  (void)module;
  TgcString s = s_BuildHTPattern(pattern, masks, length);
  HTAsmSig sig = { s.c_str(), HTSigScanType_Direct, 0 };
  return (uintptr_t)::HTSigScan(&sig);
}

uintptr_t CreateHook(
  cstring tag,
  uintptr_t func,
  uintptr_t detour,
  uintptr_t *trampoline
) {
  HMODULE hModule = GetCurrentModuleHandle();
  if (!hModule) {
    LOGW("CreateHook(%s): no module handle", tag);
    return 0;
  }

  HTAsmFunction sfn = { tag, (void *)func, (void *)detour, nullptr };
  HTStatus s = ::HTAsmHookCreate(hModule, &sfn);
  if (s != HT_SUCCESS || !sfn.fn) {
    LOGW(
      "CreateHook(%s) failed: error %d",
      tag,
      (int)::HTGetLastError());
    return 0;
  }

  // HTAsmHookCreate writes the trampoline into the function descriptor, but
  // some loader builds only fill it after the hook is enabled.
  if (trampoline)
    *trampoline = (uintptr_t)sfn.origin;
  return (uintptr_t)sfn.fn;
}

void DeleteHook(
  uintptr_t hHook
) {
  (void)hHook;
  // HTModLoader didn't provide any API to remove a hook.
}

bool EnableHook(
  uintptr_t hHook
) {
  HMODULE hModule = GetCurrentModuleHandle();
  if (!hModule || !hHook)
    return false;

  HTStatus s = ::HTAsmHookEnable(hModule, (void *)hHook);
  if (s != HT_SUCCESS) {
    LOGW("EnableHook failed: error %d", (int)::HTGetLastError());
    return false;
  }
  return true;
}

bool DisableHook(
  uintptr_t hHook
) {
  HMODULE hModule = GetCurrentModuleHandle();
  if (!hModule || !hHook)
    return false;

  HTStatus s = ::HTAsmHookDisable(hModule, (void *)hHook);
  if (s != HT_SUCCESS) {
    LOGW("DisableHook failed: error %d", (int)::HTGetLastError());
    return false;
  }
  return true;
}

} // namespace HTModLoader
