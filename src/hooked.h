#ifndef __HOOKED_H__
#define __HOOKED_H__

#include "aliases.h"
#include "setup.h"
#include "types.h"

#ifdef __cplusplus
extern "C" {
#endif

// Typedefs of trampoline functions.
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
typedef u64 (__fastcall *FnInput_getMouseDeltaPx)(
  u64, v4f *);

extern u64 gSavedLevelContext;
extern SetupFunctions_t gTramp;

i08 initAllHooks();
i08 createAllHooks();
i08 removeAllHooks();

#ifdef __cplusplus
}
#endif

#endif
