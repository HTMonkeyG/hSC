// ----------------------------------------------------------------------------
// Preprocess configures for HT's Mod Loader.
// ----------------------------------------------------------------------------

// Game backends.
// - Determine the target game window and process using the game backend.
// - Only one backend implementation is allowed to be enabled at compile time.
//
// Sky: CotL, Chinese and international edition.
#define USE_IMPL_SKY
// Minecraft: Bedrock Edition, both netease and international.
//#define USE_IMPL_MCBE

// Graphic backends.
// - Graphic backends must be strongly related to the target game.
// - Graphic backends is used to initialize ImGui.
// - Multiple graphic backends can be enabled at the same time, but only one will be used.
//
// Vulkan layer.
#define USE_IMPL_VKLAYER
// OpenGL3.
//#define USE_IMPL_OPENGL3

#define USE_IMPL_PROXY
