// ----------------------------------------------------------------------------
// Function prototypes for mods written for HTModLoader.
// <https://www.github.com/HTMonkeyG/HTML-Sky>
//
// The mod should implement and export functions declared below.
//
// MIT License
//
// Copyright (c) 2025 HTMonkeyG
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//
// ----------------------------------------------------------------------------

// #pragma once
#ifndef __HTMOD_H__
#define __HTMOD_H__

#include <windows.h>
#include "htmodloader/htmodloader.h"

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
