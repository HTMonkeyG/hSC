#ifndef __UTILS_WINAPICALL_HPP__
#define __UTILS_WINAPICALL_HPP__

#include <Windows.h>
#include "utils/types.hpp"

namespace Win64SysCall {

TgcWString GetModuleFolder(HMODULE module);

// Get the handle (base address) of a dynamic module.
uintptr_t GetDlModule(cstring name);

// Read process memory.
bool ReadMemory(uintptr_t addr, u08 *dst, size_t size);
// Write process memory.
bool WriteMemory(uintptr_t addr, const u08 *src, size_t size);

} // namespace Win64SysCall

#endif
