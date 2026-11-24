// ----------------------------------------------------------------------------
// Preprocess configures for HT's Mod Loader.
// <https://www.github.com/HTMonkeyG/HTML-Sky>
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

// Game backends.
// - Determine the target game window and process using the game backend.
// - Only one backend implementation is allowed to be enabled at compile time.
//
// Sky: CotL, Chinese and international edition.
#define HTML_USE_IMPL_SKY
// Minecraft: Bedrock Edition, both netease and international.
//#define HTML_USE_IMPL_MCBE

// Graphic backends.
// - Graphic backends must be strongly related to the target game.
// - Graphic backends is used to initialize ImGui.
// - Multiple graphic backends can be enabled at the same time, but only one will be used.
//
// Vulkan layer.
#define HTML_USE_IMPL_VKLAYER
// OpenGL3.
//#define HTML_USE_IMPL_OPENGL3

// Enable the logger.
//#define HTML_ENABLE_LOGGER

// Compile the debugger.
#define HTML_ENABLE_DEBUGGER
