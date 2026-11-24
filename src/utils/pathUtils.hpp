#ifndef __UTILS_PATHUTILS_HPP__
#define __UTILS_PATHUTILS_HPP__

#include <vector>
#include <filesystem>
#include <Windows.h>
#include <htmodloader/htmodloader.h>
#include "utils/types.hpp"
#include "utils/stringUtils.hpp"

namespace PathUtils {

static constexpr i32 kMaxPathLen = 1024;

// Get the mod's installation directory.
//
// NOTE: This function will set the error code of HTModLoader.
HTStatus GetModFolder(
  TgcWString &modFolder,
  HMODULE hModuleDll);

// Check whether `dest` is inside the `src` folder.
bool IsWithin(
  const TgcWString &dest,
  const TgcWString &src);
  
// Returns the relative path from src to dest.
TgcWString Relative(
  const TgcWString &dest,
  const TgcWString &src);

// Concat path segments together.
TgcWString Join(
  const std::vector<TgcWString> &segments);

// Return the filename of a path.
inline TgcWString FileName(
  const TgcWString &fullPath
) {
  std::filesystem::path p(fullPath);
  return p.filename().wstring();
}

inline TgcWString ParentPath(
  const TgcWString &path
) {
  usize separator = path.find_last_of(L"\\/");
  if (separator == TgcWString::npos)
    return L"";
  return path.substr(0, separator);
}

// Simply add a segment to a path.
void AppendSimple(
  TgcWString &path,
  const TgcWString &s);

// Check whether `path` is an absolute path.
bool IsAbsolute(
  const TgcWString &path);

}

#endif
