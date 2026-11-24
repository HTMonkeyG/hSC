#include <Windows.h>
#include "utils/fsUtils.hpp"
#include "utils/PathUtils.hpp"

namespace FsUtils {

TgcString FsUtils::ReadFileRaw(
  const TgcWString &path
) {
  HANDLE hFile = CreateFileW(
    path.c_str(),
    GENERIC_READ,
    FILE_SHARE_READ,
    NULL,
    OPEN_EXISTING,
    FILE_ATTRIBUTE_NORMAL,
    nullptr);

  if (hFile == INVALID_HANDLE_VALUE)
    goto ErrRet;

  {
    LARGE_INTEGER fileSize;
    if (!GetFileSizeEx(hFile, &fileSize))
      goto ErrRet;

    if (fileSize.QuadPart > MAXDWORD)
      goto ErrRet;

    u32 targetSize = (u32)fileSize.QuadPart;
    TgcString content(targetSize, '\0');

    DWORD bytesRead = 0;
    if (!::ReadFile(hFile, &content[0], targetSize, &bytesRead, NULL))
      goto ErrRet;
    if (bytesRead != targetSize)
      goto ErrRet;

    CloseHandle(hFile);

    return content;
  }

ErrRet:
  if (hFile != INVALID_HANDLE_VALUE)
    CloseHandle(hFile);

  return "";
}

TgcString ReadFileUtf8(const TgcWString &path) {
  FILE *file = nullptr;
  if (_wfopen_s(&file, path.c_str(), L"rb") != 0 || !file)
    return "";

  fseek(file, 0, SEEK_END);
  long size = ftell(file);
  fseek(file, 0, SEEK_SET);

  TgcString data;
  if (size > 0) {
    data.resize(static_cast<usize>(size));
    fread(&data[0], 1, static_cast<usize>(size), file);
  }

  fclose(file);
  return data;
}

bool WriteFileUtf8(const TgcWString &path, const TgcString &data) {
  FILE *file = nullptr;
  if (_wfopen_s(&file, path.c_str(), L"wb") != 0 || !file)
    return false;

  bool ok = data.empty() || fwrite(data.data(), 1, data.size(), file) == data.size();
  fclose(file);
  return ok;
}

bool EnsureDirectory(const TgcWString &path) {
  if (path.empty())
    return false;

  std::wstring normalized = path;
  for (wchar_t &c : normalized) {
    if (c == L'/')
      c = L'\\';
  }

  usize start = 0;
  if (normalized.size() >= 2 && normalized[1] == L':')
    start = 3;
  else if (normalized.size() >= 2 && normalized[0] == L'\\' &&
           normalized[1] == L'\\')
    start = 2;

  for (usize i = start; i <= normalized.size(); ++i) {
    if (i != normalized.size() && normalized[i] != L'\\')
      continue;
    if (i == 0)
      continue;

    std::wstring part = normalized.substr(0, i);
    if (part.size() == 2 && part[1] == L':')
      continue;

    CreateDirectoryW(part.c_str(), nullptr);
  }

  DWORD attributes = GetFileAttributesW(normalized.c_str());
  return attributes != INVALID_FILE_ATTRIBUTES &&
         (attributes & FILE_ATTRIBUTE_DIRECTORY) != 0;
}

} // namespace FsUtils
