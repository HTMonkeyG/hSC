#include <Windows.h>

#include "utils/utf8Utils.hpp"

namespace Utf8Utils {

TgcWString ToWide(
  const TgcString &text
) {
  if (text.empty())
    return TgcWString();

  int length = MultiByteToWideChar(
    CP_UTF8, 0, text.c_str(), static_cast<int>(text.size()), nullptr, 0);
  if (length <= 0)
    return TgcWString();

  TgcWString result(static_cast<usize>(length), L'\0');
  MultiByteToWideChar(
    CP_UTF8, 0, text.c_str(), static_cast<int>(text.size()), &result[0], length);
  return result;
}

TgcString ToUtf8(
  const TgcWString &text
) {
  if (text.empty())
    return TgcString();

  int length = WideCharToMultiByte(
    CP_UTF8, 0, text.c_str(), static_cast<int>(text.size()),
    nullptr, 0, nullptr, nullptr);
  if (length <= 0)
    return TgcString();

  TgcString result(static_cast<usize>(length), '\0');
  WideCharToMultiByte(
    CP_UTF8, 0, text.c_str(), static_cast<int>(text.size()),
    &result[0], length, nullptr, nullptr);
  return result;
}

} // namespace Utf8Utils
