#include <math.h>

#include "internal.h"

// Defines.
#define MH_SUCCESSED(v, s) ((v) |= (!(s)))

// ----------------------------------------------------------------------------
// [SECTION] Definitions and declarations.
// ----------------------------------------------------------------------------

// Static variables.
static FuncAddresses gFunc;
static i08 gInit = 0;

// Global variables.
FuncAddresses gTramp = {0};
u64 gSavedLevelContext = 0;

// ----------------------------------------------------------------------------
// [SECTION] Detour functions.
// ----------------------------------------------------------------------------

/**
 * Detour function for SkyCameraProp::update().
 * 
 * This is the main update function of the camera prop.
 */
static u64 hook_SkyCameraProp_update(SkyCameraProp *this, u64 context) {
  u64 result;
  // NOTE: We should NOT save the SkyCameraProp *this variable due to it may vary
  // whenever. Every frame the update should be presented in the detour
  // function only.
  result = ((PFN_SkyCameraProp_update)gTramp.fn_SkyCameraProp_update)(
    this, context);
  return result;
}

/**
 * Detour function for SkyCameraProp::_updateParams().
 * 
 * The original function calculates the camera position and facing direction.
 */
static u64 hook_SkyCameraProp__updateParams(SkyCameraProp *this, u64 context) {
  u64 result;

  result = ((PFN_SkyCameraProp__updateParams)gTramp.fn_SkyCameraProp__updateParams)(
    this, context);
  
  if (this->cameraType == gContext.cameraMode + 1)
    hscUpdatePropMain(this);

  return result;
}

/**
 * Detour function for SkyCameraProp::updateUI().
 * 
 * The original function renders the camera UI on the screen.
 */
static u64 hook_SkyCameraProp_updateUI(
  SkyCameraProp *this,
  u64 a2,
  u64 a3,
  u64 a4,
  f32 *scale,
  f32 *focus,
  f32 *brightness,
  u64 a8,
  i08 a9
) {
  u64 result;
  if (gContext.enable && gContext.noOriginalUi)
    // Disable original camera ui.
    return 0;
  result = ((PFN_SkyCameraProp_updateUI)gTramp.fn_SkyCameraProp_updateUI)(
    this, a2, a3, a4, scale, focus, brightness, a8, a9);
  return result;
}

/**
 * Detour function for World::interactionCheck().
 * 
 * The original function is executing the interaction check for a line and
 * current level.
 * 
 * The pointer passed into this function must be a local variable address.
 * 
 * Param a5 and a6 is missed when use IDA to decompile.
 */
static u64 hook_World_interactionTest(
  u64 a1,
  v4f *origin,
  v4f *direction,
  float a4,
  v4f *a5,
  i08 *a6
) {
  u64 result;
  if (gSavedLevelContext != a1 && gInit) {
    // Save pointer to current context.
    gSavedLevelContext = a1;
    LOGI("World::interactionTest(): context: %p\n", (void *)gSavedLevelContext);
  }
  result = ((PFN_World_interactionTest)gTramp.fn_Level_interactionTest)(
    a1, origin, direction, a4, a5, a6);
  return result;
}

/**
 * Detour function for WhiskerCamera::update().
 * 
 * The original function updates the default camera which mouse controls.
 */
static u64 hook_WhiskerCamera_update(
  WhiskerCamera *this,
  u64 *context
) {
  u64 result;
  result = ((PFN_WhiskerCamera_update)gTramp.fn_WhiskerCamera_update)(
    this, context);
  if (gContext.cameraMode == HscCameraMode_Whisker) {
    hscPreupdateCameraMain(&this->super);
    hscUpdateCameraMain(&this->super);
  }
  return result;
}

/**
 * Detour function for SkyCamera::update().
 * 
 * The original function calculates the rotation matrix from the orientation
 * vector.
 */
static u64 hook_SkyCamera_update(
  SkyCamera *this,
  u64 *context
) {
  u64 result;
  result = ((PFN_SkyCamera_update)gTramp.fn_SkyCamera_update)(
    this, context);
  if (
    gContext.cameraMode < HscCameraMode_Whisker
    && SkyCamera_getPropPtr(this)->cameraType == gContext.cameraMode + 1
  ) {
    hscPreupdateCameraMain(&this->super);
    hscUpdateCameraMain(&this->super);
  }
  return result;
}

/**
 * Detour function of Input::getMouseDeltaPx().
 * 
 * The original function copies the mouse delta value from the global Input
 * object to `delta`.
 */
static u64 hook_Input_getMouseDeltaPx(u64 a1, v4f *delta) {
  u64 result;
  result = ((PFN_Input_getMouseDeltaPx)gTramp.fn_Input_getMouseDeltaPx)(
    a1, delta);
  gMouseDeltaPx = *delta;
  return result;
}

// ----------------------------------------------------------------------------
// [SECTION] Initializations.
// ----------------------------------------------------------------------------

static void *const detourFunc[9] = {
  hook_SkyCameraProp__updateParams,
  hook_SkyCameraProp_updateUI,
  NULL,
  hook_SkyCameraProp_update,
  NULL,
  hook_World_interactionTest,
  hook_WhiskerCamera_update,
  hook_SkyCamera_update,
  hook_Input_getMouseDeltaPx
};

/**
 * Scan functions with signature.
 */
i08 hscInitAllHooks() {
  return hscSetupFuncWithSig(&gFunc);
}

/**
 * Hook all the functions that we need.
 */
i08 hscCreateAllHooks() {
  HTStatus s;
  void *p;
  i32 length;
  i08 r = 1;

  if (gInit)
    return 1;

  length = sizeof(detourFunc) / sizeof(void *);
  for (i32 i = 0; i < length; i++) {
    if (!gFunc.functions[i] || !detourFunc[i])
      continue;

    HTAsmFunction fn = {
      .fn = gFunc.functions[i],
      .name = RequiredFn[i]->name,
      .detour = detourFunc[i]
    };
    p = gFunc.functions[i];
    s = HTAsmHookCreate(hModuleDll, &fn);
    gTramp.functions[i] = fn.origin;

    if (!s) {
      LOGE("Create hook on 0x%p failed: %d.\n", p, HTGetLastError());
      r = 0;
    } else {
      LOGI("Created hook on 0x%p\n", p);
      s = HTAsmHookEnable(hModuleDll, p);
      if (!s) {
        LOGE("Enable hook on 0x%p failed.\n", p);
        r = 0;
      } else
        LOGI("Enabled hook on 0x%p\n", p);
    }
  }

  gInit = 1;

  return r;
}
