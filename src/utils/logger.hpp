#ifndef __UTILS_LOGGER_HPP__
#define __UTILS_LOGGER_HPP__

#include <htmodloader/htmodloader.h>

#include <fstream>
#include <iostream>
#include <mutex>
#include "utils/types.hpp"

#define LOG(msg, ...) GetLogger()->Log(nullptr, (msg), ## __VA_ARGS__)
#define LOGI(msg, ...) GetLogger()->Log("[I]", (msg), ## __VA_ARGS__)
#define LOGW(msg, ...) GetLogger()->Log("[W]", (msg), ## __VA_ARGS__)
#define LOGE(msg, ...) GetLogger()->Log("[E]", (msg), ## __VA_ARGS__)

class Logger {
public:
  Logger() = default;
  ~Logger() = default;

  void Initialize(const std::wstring &path);
  void Terminate();

  void Log(cstring tag, cstring format, ...);
  void Logv(cstring tag, cstring format, va_list va);

public:
  std::wstring m_path = L"";
  std::ofstream m_fstream = {};
  std::mutex m_mutex = {};
};

Logger *GetLogger();

#endif
