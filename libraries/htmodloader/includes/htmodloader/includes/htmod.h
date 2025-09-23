// ----------------------------------------------------------------------------
// The mod should implement and export functions declared below.
// ----------------------------------------------------------------------------

// #pragma once
#ifndef __HTMOD_H__
#define __HTMOD_H__

#include <windows.h>
#include "includes/htmodloader.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * ImGui draw calls should only be put in this function. `timeElapesed`
 * indicates the time elapsed since the last frame.
 */
__declspec(dllexport) void HTMLAPI HTModRenderGui(
  f32 timeElapesed, void *reserved);
__declspec(dllexport) HTStatus HTMLAPI HTModOnInit(
  void *reserved);
__declspec(dllexport) HTStatus HTMLAPI HTModOnEnable(
  void *reserved);

#ifdef __cplusplus
}
#endif

#endif
