#ifndef __INTERNAL_H__
#define __INTERNAL_H__

#include <windows.h>

#include "aliases.h"
#include "mth/macros.h"
#include "mth/vector.h"
#include "mth/matrix.h"
#include "mth/euler.h"
#include "mth/aabb.h"

#ifdef __cplusplus
extern "C" {
#endif

//-----------------------------------------------------------------------------
// [SECTION] Globals.
//-----------------------------------------------------------------------------

extern HMODULE hModuleDll;

//-----------------------------------------------------------------------------
// [SECTION] Type definitions.
//-----------------------------------------------------------------------------

typedef enum {
  FIRST = 1,
  FRONT = 2,
  PLACE = 3
} SkyCameraType;

// Stores the status of the camera prop.
typedef struct {
  i64 lpVtbl;
  i32 field_8;
  i08 field_C;
  i64 field_D;
  i64 field_E;
  i08 field_15;
  i32 field_16;
  i08 field_1E;
  i32 unk_1;
  i32 field_1F;
  i32 field_23;
  i08 cameraType;
  i08 unk_2[15];
  u64 player;
  i08 unk_2_2[56];
  u64 *unk_2_3;
  u64 *unk_2_4;
  u64 *unk_2_5;
  i08 unk_2_6[13];
  v4f cameraPos;
  i08 unk_3[64];
  f32 rotateXAnim;
  f32 rotateYAnim;
  i08 unk_4[8];
  f32 rotateSpeedX;
  f32 rotateSpeedY;
  i08 unk_4_2[8];
  f32 rotateX;
  f32 rotateY;
  u64 unk_4_3;
  f32 scale;
  f32 scaleAnim;
  i08 unk_4_4[24];
  f32 focus;
  f32 focusAnim;
  u64 unk_4_5;
  f32 brightness;
  f32 brightnessAnim;
  i08 unk_5[88];
  v4f lookAt;
  i08 unk_6[40];
  f32 unk_7;
  i08 unk_8[20];
  i32 *unk_8_2;
  i08 unk_8_3[5];
} SkyCameraProp;

typedef struct {
  u32 unk_1;
  // The field-of-view in the unit of 'rad'.
  f32 fov;
  u64 unk_2;
  // Rotate matrix (3x3) is stored separately by rows in three vectors. The
  // last component of these vectors is 0.
  v4f mat1;
  v4f mat2;
  v4f mat3;
  // The last element of this vector is 1. We can consider the four vectors
  // as a single matrix (4x4).
  v4f cameraPos;
  char unk_4[32];
} MainCameraContext;

// The parent class of all possible camera updates.
typedef struct MainCamera {
  u64 *lpVtbl;
  u64 unk_1;
  u64 unk_2;
  struct MainCamera *prev;
  struct MainCamera *next;
  u64 unk_3;
  MainCameraContext context1;
  MainCameraContext context2;
  u64 active;
} MainCamera;

// The size of the following two structs is incorrect. This definition only
// contains components what we need.
typedef struct {
  MainCamera super;
  char unk_1[56];
  v4f mouseDelta;
  char unk_2[1456];
} WhiskerCamera;

typedef struct {
  MainCamera super;
  void *prop;
  void *unk_2;
  void *unk_3;
  v4f pos;
  v4f dir;
} SkyCamera;

#define SkyCamera_getPropPtr(sc) ((SkyCameraProp *)((i08 *)(((SkyCamera *)(sc))->prop) - 0x88))

typedef struct {
  v4f intersection;
  v4f normalize;
  i08 unk[0x40];
} InteractionResult;

//-----------------------------------------------------------------------------
// [SECTION] Signature scanner of hSC.
//-----------------------------------------------------------------------------

#ifndef HTML_VERSION
// Method for obtaining the final address.
typedef enum {
  // The Signature represents the function body.
  HT_SCAN_DIRECT = 0,
  // The signature represents the E8 or E9 instruction that calls the function.
  HT_SCAN_E8,
  // The signature represents the FF15 instruction that calls the function.
  HT_SCAN_FF15,
} HTSigScanType;

// Signature code config.
typedef struct {
  // Signature code.
  const char *sig;
  // Function name, only for debug use.
  const char *name;
  // Method for obtaining the final address.
  HTSigScanType indirect;
  // The byte offset of 0xE8 or 0x15 byte for HT_SCAN_E8 and HT_SCAN_FF15, or
  // the byte offset to the first instruction for HT_SCAN_DIRECT.
  i32 offset;
} HTSignature;
#endif

void *sigScan(const char *moduleName, const char *sig, i32 offset);
void *sigScanE8(const char *moduleName, const char *sig, i32 offset);
i08 sigCheckProcess(void *addr, char *sig);

//-----------------------------------------------------------------------------
// [SECTION] Plugin setup functions.
//-----------------------------------------------------------------------------

typedef union {
  struct {
    // SkyCameraProp functions.
    void *fn_SkyCameraProp__updateParams;
    void *fn_SkyCameraProp_updateUI;
    void *fn_SkyCameraProp_setState;
    void *fn_SkyCameraProp_update;

    // Player functions.
    void *fn_Player_getCameraPos;

    // Level functions.
    void *fn_Level_interactionTest;

    // WhiskerCamera functions.
    void *fn_WhiskerCamera_update;

    // SkyCamera functions.
    void *fn_SkyCamera_update;

    // Input functions.
    void *fn_Input_getMouseDeltaPx;
  };

  // Array of functions.
  void *functions[9];
} SetupFunctions_t;

i08 setupFuncWithSig(SetupFunctions_t *functions);
i08 setupPaths(HMODULE hModule, wchar_t *prefPath, char *guiIniPath);

//-----------------------------------------------------------------------------
// [SECTION] Hook installer and hooked function type declarations.
//-----------------------------------------------------------------------------

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

//-----------------------------------------------------------------------------
// [SECTION] Camera updater of hSC.
//-----------------------------------------------------------------------------

typedef struct {
  MainCamera super;

  i08 useFov;
  i08 useMatrix;
  i08 usePos;
} InjectCamera;

extern InjectCamera gInjectCamera;

void updatePropMain(SkyCameraProp *);
void preupdateCameraMain(MainCamera *);
void updateCameraMain(MainCamera *);

#ifdef __cplusplus
}
#endif

#endif

