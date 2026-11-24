#include <stdio.h>
#include <stdarg.h>
#include "utils/types.hpp"
#include "utils/assert.hpp"

static PFN_AssertHandler g_assertHandler = nullptr;

void Private::AssertImpl(
  cstring expr,
  cstring file,
  u32 line,
  u32 column
) {
  if (g_assertHandler)
    g_assertHandler(expr, "", file, line, column);
}

void Private::AssertMsgImpl(
  cstring expr,
  cstring file,
  u32 line,
  u32 column,
  cstring format,
  ...
) {
  char buffer[2056];

  if (!g_assertHandler)
    return;

  va_list va;
  va_start(va, format);
  vsnprintf(buffer, sizeof(buffer), format, va);
  va_end(va);

  g_assertHandler(expr, buffer, file, line, column);
}

void Private::AssertMsgImplV(
  cstring expr,
  cstring file,
  u32 line,
  u32 column,
  cstring format,
  va_list va
) {
  char buffer[2056];

  if (!g_assertHandler)
    return;

  vsnprintf(buffer, sizeof(buffer), format, va);

  g_assertHandler(expr, buffer, file, line, column);
}

void SetAssertHandler(
  PFN_AssertHandler handler
) {
  g_assertHandler = handler;
}
