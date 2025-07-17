#include <math.h>
#include "MinHook.h"

#include "types.h"
#include "hooked.h"
#include "log.h"
#include "setup.h"
#include "camera.h"
#include "mth/vector.h"
#include "mth/matrix.h"
#include "ui/gui.h"

// Defines.
#define MH_SUCCESSED(v, s) ((v) |= (!(s)))

// ----------------------------------------------------------------------------
// [SECTION] Definitions and declarations.
// ----------------------------------------------------------------------------

// Typedefs.
typedef u64 (__fastcall *FnSkyCameraProp_update)(SkyCameraProp *, u64);
typedef u64 (__fastcall *FnSkyCameraProp__updateParams)(SkyCameraProp *, u64);
typedef u64 (__fastcall *FnSkyCameraProp_updateUI)(
  SkyCameraProp *, u64, u64, u64, f32 *, f32 *, f32 *, u64, i08);
typedef u64 (__fastcall *FnWorld_interactionTest)(
  u64, v4f *, v4f *, float, v4f *, i08 *);
typedef u64 (__fastcall *FnWhiskerCamera_update)(
  WhiskerCamera *, u64 *);
typedef u64 (__fastcall *FnSkyCamera_update)(
  SkyCamera *, u64 *);
typedef u64 (__fastcall *FnMainCamera__getDelta)(
  u64, i08 *, u64, u64, u64, int, int);

// External variables.
// gSavedLevelContext defined in camera.c
extern u64 gSavedLevelContext;

// Static variables.
static SetupFunctions_t gFunc;
static i08 gInit = 0;

// Global variables.
SetupFunctions_t gTramp = {0};

// ----------------------------------------------------------------------------
// [SECTION] Detour functions.
// ----------------------------------------------------------------------------

/**
 * Detour function for SkyCameraProp::update().
 * 
 * This is the main update function of the camera prop.
 */
static u64 SkyCameraProp_update_Listener(SkyCameraProp *this, u64 context) {
  u64 result;
  // NOTE: We should NOT save the SkyCameraProp *this variable due to it may vary
  // whenever. Every frame the update should be presented in the detour
  // function only.
  result = ((FnSkyCameraProp_update)gTramp.fn_SkyCameraProp_update)(
    this, context);
  return result;
}

/**
 * Detour function for SkyCameraProp::_updateParams().
 * 
 * The original function calculates the camera position and facing direction.
 */
static u64 SkyCameraProp__updateParams_Listener(SkyCameraProp *this, u64 context) {
  u64 result;

  result = ((FnSkyCameraProp__updateParams)gTramp.fn_SkyCameraProp__updateParams)(
    this, context);
  
  if (this->cameraType == gState.cameraMode + 1)
    updatePropMain(this);

  return result;
}

/**
 * Detour function for SkyCameraProp::updateUI().
 * 
 * The original function renders the camera UI on the screen.
 */
static u64 SkyCameraProp_updateUI_Listener(
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
  if (gState.noOriginalUi)
    // Disable original camera ui.
    return 0;
  result = ((FnSkyCameraProp_updateUI)gTramp.fn_SkyCameraProp_updateUI)(
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
static u64 World_interactionTest_Listener(
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
  result = ((FnWorld_interactionTest)gTramp.fn_Level_interactionTest)(
    a1, origin, direction, a4, a5, a6);
  return result;
}

/**
 * Detour function for WhiskerCamera::update().
 * 
 * The original function updates the default camera which mouse controls.
 */
static u64 WhiskerCamera_update_Listener(
  WhiskerCamera *this,
  u64 *context
) {
  u64 result;
  result = ((FnWhiskerCamera_update)gTramp.fn_WhiskerCamera_update)(
    this, context);
  if (gState.cameraMode == CM_WHISKER) {
    preupdateCameraMain(&this->super);
    updateCameraMain(&this->super);
  }
  return result;
}

/**
 * Detour function for SkyCamera::update().
 * 
 * The original function calculates the rotation matrix from the orientation
 * vector.
 */
static u64 SkyCamera_update_Listener(
  SkyCamera *this,
  u64 *context
) {
  u64 result;
  result = ((FnSkyCamera_update)gTramp.fn_SkyCamera_update)(
    this, context);
  if (
    gState.cameraMode < CM_WHISKER
    && SkyCamera_getPropPtr(this)->cameraType == gState.cameraMode + 1
  ) {
    preupdateCameraMain(&this->super);
    updateCameraMain(&this->super);
  }
  return result;
}

/**
 * Detour function for MainCamera::_getdelta().
 * 
 * The original function calculates the mouse delta on last frame.
 */
static u64 MainCamera__getDelta_Listener(
  u64 a1,
  i08 *a2,
  u64 a3,
  u64 a4,
  u64 a5,
  int a6,
  int a7
) {
  u64 result;
  result = ((FnMainCamera__getDelta)gTramp.fn_MainCamera__getDelta)(
    a1, a2, a3, a4, a5, a6, a7);
  if (a2[1])
    updateMouseDelta(*(v4f *)(a2 + 0x10));
  else
    updateMouseDelta(v4fnew(0, 0, 0, 0));;
  return result;
}

/**
 * Detour function of Input::getMouseDeltaPx().
 * 
 * The original function copies the mouse delta value from the global Input
 * object to `delta`.
 */
static u64 Input_getMouseDeltaPx_Listener(u64 *a1, v4f *delta) {
  return 0;
}

// ----------------------------------------------------------------------------
// [SECTION] Initializations.
// ----------------------------------------------------------------------------

static const void *detourFunc[9] = {
  SkyCameraProp__updateParams_Listener,
  SkyCameraProp_updateUI_Listener,
  NULL,
  SkyCameraProp_update_Listener,
  NULL,
  World_interactionTest_Listener,
  WhiskerCamera_update_Listener,
  SkyCamera_update_Listener,
  MainCamera__getDelta_Listener
};

/**
 * Scan functions with signature.
 */
i08 initAllHooks() {
  return setupFuncWithSig(&gFunc);
}

/**
 * Hook all the functions that we need.
 */
i08 createAllHooks() {
  MH_STATUS s;
  i32 length;
  i08 r = 1;
  void *fn;

  if (gInit)
    return 1;

  length = sizeof(detourFunc) / sizeof(void *);
  for (i32 i = 0; i < length; i++) {
    if (!gFunc.functions[i] || !detourFunc[i])
      continue;

    fn = gFunc.functions[i];
    s = MH_CreateHook(
      fn,
      (void *)detourFunc[i],
      &gTramp.functions[i]);

    if (s) {
      LOGE("Create hook on 0x%p failed.\n", fn);
      r = 0;
    } else {
      LOGI("Created hook on 0x%p\n", fn);
      s = MH_EnableHook(fn);
      if (s) {
        LOGE("Enable hook on 0x%p failed.\n", fn);
        r = 0;
      } else
        LOGI("Enabled hook on 0x%p\n", fn);
    }
  }

  gInit = 1;

  return r;
}

/**
 * Remove hooks on the functions.
 */
i08 removeAllHooks() {
  MH_STATUS s;
  i32 length;
  i08 r = 1;
  void *fn;

  length = sizeof(detourFunc) / sizeof(void *);
  for (i32 i = 0; i < length; i++) {
    fn = gFunc.functions[i];
    if (!fn || !gTramp.functions[i])
      continue;
    s = MH_RemoveHook(fn);
    if (s) {
      LOGE("Remove hook on 0x%p failed.\n", fn);
      r = 0;
    } else
      LOGI("Removed hook on 0x%p\n", fn);
  }

  gInit = 0;

  return r;
}
