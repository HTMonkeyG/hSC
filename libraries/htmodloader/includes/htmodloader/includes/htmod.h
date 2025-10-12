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
  float timeElapesed, void *reserved);

/**
 * Mods can only call HTML APIs within and after this function.
 */
__declspec(dllexport) HTStatus HTMLAPI HTModOnInit(
  void *reserved);

/**
 * [Future] Mod can put its clean-up procedures within this function.
 */
__declspec(dllexport) HTStatus HTMLAPI HTModOnDeinit(
  void *reserved);

/**
 * When this function is called, the mod is marked as "enabled", mod can start
 * all of its procedures.
 * 
 * NOTE: This function may not call by the same thread as HTModOnInit().
 */
__declspec(dllexport) HTStatus HTMLAPI HTModOnEnable(
  void *reserved);

/**
 * [Future] When this function is called, the mod is marked as "disabled",
 * the mod should disable all of its functions.
 */
__declspec(dllexport) HTStatus HTMLAPI HTModOnDisable(
  void *reserved);

#ifdef __cplusplus
}
#endif

#endif
