#include <windows.h>
#include <stdarg.h>

#include "utils/log.h"
#include "internal.h"

void recreateConsole() {
#ifdef DEBUG_CONSOLE
  FreeConsole();
  AllocConsole();
  freopen("CONOUT$", "w+t", stdout);
  freopen("CONIN$", "r+t", stdin);
#else
  ;
#endif
}

void logImp(const char *format, ...) {
  SYSTEMTIME time = {0};
  va_list arg;

  va_start(arg, format);
  GetLocalTime(&time);
  printf(
    "[%04d-%02d-%02d %02d:%02d:%02d.%03d]",
    time.wYear,
    time.wMonth,
    time.wDay,
    time.wHour,
    time.wMinute,
    time.wSecond,
    time.wMilliseconds
  );
  vprintf(format, arg);
  fflush(stdout);
  va_end(arg);
}

void wlogImp(const wchar_t *format, ...) {
  SYSTEMTIME time = {0};
  va_list arg;

  va_start(arg, format);
  GetLocalTime(&time);
  wprintf(
    L"[%04d-%02d-%02d %02d:%02d:%02d.%03d]",
    time.wYear,
    time.wMonth,
    time.wDay,
    time.wHour,
    time.wMinute,
    time.wSecond,
    time.wMilliseconds
  );
  vwprintf(format, arg);
  fflush(stdout);
  va_end(arg);
}
