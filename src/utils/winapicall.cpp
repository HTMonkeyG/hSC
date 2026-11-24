#include <Windows.h>
#include "utils/winapicall.hpp"

// ----------------------------------------------------------------------------
// [SECTION] OsApi
// ----------------------------------------------------------------------------

namespace Win64SysCall {

TgcWString GetModuleFolder(HMODULE module) {
  wchar_t path[MAX_PATH] = {0};
  DWORD length = GetModuleFileNameW(module, path, MAX_PATH);

  if (length == 0)
    return L".";

  std::wstring result(path, length);
  usize separator = result.find_last_of(L"\\/");
  if (separator == std::wstring::npos)
    return L".";

  return result.substr(0, separator);
}

uintptr_t GetDlModule(cstring name) {
  return (uintptr_t)::GetModuleHandleA(name);
}

bool ReadMemory(
  uintptr_t addr,
  u08 *dst,
  size_t size
) {
  SIZE_T read = 0;
  if (!::ReadProcessMemory(
    ::GetCurrentProcess(), (void *)addr, dst, size, &read) ||
    read != size)
  {
    return false;
  }
  return true;
}

bool WriteMemory(
  uintptr_t addr,
  const u08 *src,
  size_t size
) {
  SIZE_T written = 0;
  if (!::WriteProcessMemory(
    ::GetCurrentProcess(), (void *)addr, src, size, &written) ||
    written != size)
  {
    return false;
  }
  return true;
}

} // namespace Win64SysCall
