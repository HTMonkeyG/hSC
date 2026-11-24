#include <filesystem>
#include <htmodloader/htmodloader.h>
#include "utils/pathUtils.hpp"

using path = std::filesystem::path;

HTStatus PathUtils::GetModFolder(
  TgcWString &modFolder,
  HMODULE hModule
) {
  HTHandle hManifest = HTGetModManifest(hModule);
  if (!hManifest)
    return HT_FAIL;

  u32 length = HTGetModInfoFrom(hManifest, HTModInfoFields_Folder, nullptr, 0);
  if (!length)
    return HT_FAIL;

  modFolder.resize(length);
  if (!HTGetModInfoFrom(hManifest, HTModInfoFields_Folder, modFolder.data(), length))
    return HT_FAIL;

  return HT_SUCCESS;
}

bool PathUtils::IsWithin(
  const TgcWString &dest,
  const TgcWString &src
) {
  wchar_t buffer[kMaxPathLen];

  HTPathRelative(buffer, src.c_str(), dest.c_str(), kMaxPathLen);
  if (HTPathIsAbsolute(buffer) || !memcmp(buffer, L"..", 2))
    return false;

  return true;
}

TgcWString PathUtils::Relative(
  const TgcWString &dest,
  const TgcWString &src
) {
  u32 length = HTPathRelative(nullptr, src.c_str(), dest.c_str(), 0x7FFFFFFF);
  if (!length)
    return L"";

  TgcWString result(length - 1, L'\0');
  if (!HTPathRelative(result.data(), src.c_str(), dest.c_str(), length))
    return L"";

  return result;
}

TgcWString PathUtils::Join(
  const std::vector<TgcWString> &segments
) {
  std::vector<const wchar_t *> list(segments.size() + 1, nullptr);

  for (u32 i = 0; i < segments.size(); i++)
    list[i] = segments[i].c_str();

  // We set maxLength to a large number to avoid a bug (#8) of HTModLoader.
  u32 length = HTPathJoin(nullptr, list.data(), 0x7FFFFFFF);
  if (!length)
    return L"";

  TgcWString result(length - 1, L'\0');
  if (!HTPathJoin(result.data(), list.data(), length))
    return L"";

  return result;
}

void PathUtils::AppendSimple(
  TgcWString &path,
  const TgcWString &s
) {
  int c = 0;
  if (path[path.length() - 1] == L'\\' || path[path.length() - 1] == L'/')
    c++;
  if (s[0] == L'\\' || s[0] == L'/')
    c++;

  if (c == 2)
    path += s.substr(1);
  else if (c == 1)
    path += s;
  else
    path += L'\\' + s;
}

bool PathUtils::IsAbsolute(
  const TgcWString &path
) {
  return HTPathIsAbsolute(path.c_str());
}
