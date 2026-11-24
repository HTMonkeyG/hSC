#ifndef __UTILS_ASSERT_HPP__
#define __UTILS_ASSERT_HPP__

#include <stdlib.h>
#include <stdarg.h>
#include "utils/types.hpp"

namespace Private {

// Extract the filename from the __FILE__ macro.
inline constexpr cstring FileName(cstring path) {
#ifdef __INTELLISENSE__
  // Intellisense stub.
  //
  // VS2022 IntelliSense will throw error E0028 when a constexpr function returns
  // a pointer to the interior of a string literal. Therefore, add a stub that
  // directly returns path to prevent error message pollution.
  return path;
#else
  const char *start = path;
  for (const char *p = path; *p != '\0'; ++p) {
    if (*p == '/' || *p == '\\') {
      start = p + 1;
    }
  }
  return start;
#endif
}

void AssertImpl(
  cstring expr,
  cstring file,
  u32 line,
  u32 column);

void AssertMsgImpl(
  cstring expr,
  cstring file,
  u32 line,
  u32 column,
  cstring format,
  ...);

void AssertMsgImplV(
  cstring expr,
  cstring file,
  u32 line,
  u32 column,
  cstring format,
  va_list va);

}

using PFN_AssertHandler = void (*)(cstring, cstring, cstring, u32, u32);

void SetAssertHandler(PFN_AssertHandler handler);

// NOTE: These macros never terminate the process. A mod must not crash the
// game because of its own bookkeeping, so a failed assertion is reported to the
// handler and execution continues. Callers that cannot safely continue are
// expected to check their inputs and degrade gracefully instead of relying on
// the assertion.
#define ASSERT(expr) \
  do {\
    constexpr cstring FILENAME = Private::FileName(__FILE__);\
    if (!(expr)) {\
      Private::AssertImpl(#expr, FILENAME, __LINE__, 1);\
    }\
  } while (0)

#define ASSERTMSG(expr, msg, ...) \
  do {\
    constexpr cstring FILENAME = Private::FileName(__FILE__);\
    if (!(expr)) {\
      Private::AssertMsgImpl(#expr, FILENAME, __LINE__, 1, msg, ## __VA_ARGS__);\
    }\
  } while (0)

#endif
