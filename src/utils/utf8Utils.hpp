#ifndef __UTILS_UTF8UTILS_HPP__
#define __UTILS_UTF8UTILS_HPP__

#include "utils/types.hpp"

namespace Utf8Utils {

// Convert a UTF-8 string to UTF-16. Returns an empty string on failure.
TgcWString ToWide(const TgcString &text);

// Convert a UTF-16 string to UTF-8. Returns an empty string on failure.
TgcString ToUtf8(const TgcWString &text);

} // namespace Utf8Utils

#endif
