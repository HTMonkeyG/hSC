#include <stdarg.h>
#include <stdio.h>
#include <vector>
#include "utils/types.hpp"
#include "utils/logger.hpp"

static Logger *g_Logger = nullptr;

void Logger::Initialize(const std::wstring &path) {
  if (g_Logger)
    return;

  g_Logger = this;

  m_path = path;
  m_fstream = std::ofstream(path);
}

void Logger::Terminate() {
  m_fstream.close();
}

void Logger::Log(
  cstring tag,
  cstring format,
  ...
) {
  va_list va;
  va_start(va, format);
  Logv(tag, format, va);
  va_end(va);
}

void Logger::Logv(
  cstring tag,
  cstring format,
  va_list va
) {
  if (!format) return;

  std::lock_guard<std::mutex> lock(m_mutex);
  if (!m_fstream.is_open()) return;
  if (tag) m_fstream << tag << " ";

  char stackBuf[512];
  va_list vacopy;
  va_copy(vacopy, va);
  int n = vsnprintf(stackBuf, sizeof(stackBuf), format, vacopy);
  va_end(vacopy);

  if (n < 0) {
    return;
  } else if (n < static_cast<int>(sizeof(stackBuf))) {
    m_fstream.write(stackBuf, n);
    if (stackBuf[strlen(stackBuf) - 1] != '\n')
      m_fstream << '\n';
  } else {
    std::vector<char> buf(static_cast<size_t>(n) + 1);
    vsnprintf(buf.data(), buf.size(), format, va);
    m_fstream.write(buf.data(), n);
    if (buf[buf.size() - 1] != '\n')
      m_fstream << '\n';
  }
  m_fstream.flush();
}

Logger *GetLogger() {
  return g_Logger;
}
