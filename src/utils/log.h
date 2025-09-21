#ifndef __LOG_H__
#define __LOG_H__

#include <stdio.h>

#ifndef NDEBUG
#define DEBUG_CONSOLE
#endif

#ifdef DEBUG_CONSOLE
#define LOGI(format, ...) logImp("[INFO] " format, ##__VA_ARGS__)
#define WLOGI(format, ...) wlogImp(L"[INFO] " format, ##__VA_ARGS__)
#define LOGW(format, ...) logImp("[WARN] " format, ##__VA_ARGS__)
#define WLOGW(format, ...) wlogImp(L"[WARN] " format, ##__VA_ARGS__)
#define LOGE(format, ...) logImp("[ERR] " format, ##__VA_ARGS__)
#define WLOGE(format, ...) wlogImp(L"[ERR] " format, ##__VA_ARGS__)
#define LOGEF(format, ...) logImp("[ERR][FATAL] " format, ##__VA_ARGS__)
#define WLOGEF(format, ...) wlogImp(L"[ERR][FATAL] " format, ##__VA_ARGS__)
#else
#define LOGI(format, ...) /* Nothing. */
#define WLOGI(format, ...) /* Nothing. */
#define LOGW(format, ...) /* Nothing. */
#define WLOGW(format, ...) /* Nothing. */
#define LOGE(format, ...) /* Nothing. */
#define WLOGE(format, ...) /* Nothing. */
#define LOGEF(format, ...) /* Nothing. */
#define WLOGEF(format, ...) /* Nothing. */
#endif

#ifdef __cplusplus
extern "C" {
#endif

void recreateConsole();
void logImp(const char *format, ...);
void wlogImp(const wchar_t *format, ...);

#ifdef __cplusplus
}
#endif

#endif
