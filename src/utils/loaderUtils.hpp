#ifndef __UTILS_LOADER_HPP__
#define __UTILS_LOADER_HPP__

#include <Windows.h>
#include "utils/types.hpp"

namespace HTModLoader {

HMODULE GetCurrentModuleHandle();
void SetCurrentModuleHandle(HMODULE hModule);

// Load or save mod settings.
bool LoadSetting(cstring key, TgcString *result);
bool LoadSetting(cstring key, f64 *result);
bool LoadSetting(cstring key, i32 *result);
bool LoadSetting(cstring key, bool *result);

bool SaveSetting(cstring key, const TgcString &result);
bool SaveSetting(cstring key, f64 result);
bool SaveSetting(cstring key, i32 result);
bool SaveSetting(cstring key, bool result);

// Scan the main executable with a byte/mask pattern.
uintptr_t ScanPattern(uintptr_t module, const u08 *pattern, const u08 *masks, u32 length);

// Create a hook with HTModLoader. The hook is not enabled yet.
// Returns the hook handle, or 0 on failure.
uintptr_t CreateHook(cstring tag, uintptr_t func, uintptr_t detour, uintptr_t *trampoline);
// HTModLoader has no API to delete a hook; this only disables it.
void DeleteHook(uintptr_t hHook);

bool EnableHook(uintptr_t hHook);
bool DisableHook(uintptr_t hHook);

} // namespace HTModLoader

#endif
