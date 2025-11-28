#ifndef __INTERNAL_H__
#define __INTERNAL_H__

#include <windows.h>
#include "includes/htmodloader.h"

#include "mth/macros.h"
#include "mth/vector.h"
#include "mth/matrix.h"
#include "mth/euler.h"
#include "mth/aabb.h"
#include "aliases.h"
#include "sky.h"

#define HSC_VERSION "0.4.0"

#if HTML_VERSION < 10801
#error "HTML SDK version must >= v1.8.1"
#endif

#ifdef __cplusplus
extern "C" {
#endif

//-----------------------------------------------------------------------------
// [SECTION] HSC_GLOBALS
//-----------------------------------------------------------------------------

typedef i32 HscMajorMode;
enum HscMajorMode_ {
  HscMajorMode_Static = 0,
  HscMajorMode_Dynamic = 1,
  HscMajorMode_Animation = 2,
};

typedef i32 HscCameraMode;
enum HscCameraMode_ {
  HscCameraMode_Handheld = 0,
  HscCameraMode_Selfie = 1,
  HscCameraMode_Tripod = 2,
  HscCameraMode_Whisker = 3
};
// Performance counter frequency. Initialized once by DllMain().
extern LARGE_INTEGER gPerformFreq;
// Performance counter value on the last frame.
extern LARGE_INTEGER gLastFrame;
// Time elapsed since the last frame.
extern f32 gTimeElapsed;

// The avaliable fields in a vector.
typedef i32 HscIndentAxis;
typedef enum {
  HscIndentAxis_None = 0,

  HscIndentAxis_X = 1 << 0,
  HscIndentAxis_Y = 1 << 1,
  HscIndentAxis_Z = 1 << 2,
  HscIndentAxis_W = 1 << 3,

  HscIndentAxis_All = 
    HscIndentAxis_X | HscIndentAxis_Y | HscIndentAxis_Z | HscIndentAxis_W
} HscIndentAxis_;

// The avaliable fields in a vector.
typedef i32 HscIndentBlur;
typedef enum {
  HscIndentBlur_None = 0,

  HscIndentBlur_Near = 1 << 0,
  HscIndentBlur_Far = 1 << 1,
  HscIndentBlur_StrengthNear = 1 << 2,
  HscIndentBlur_StrengthFar = 1 << 3,

  HscIndentBlur_All = 
    HscIndentBlur_Near | HscIndentBlur_Far | HscIndentBlur_StrengthNear | HscIndentBlur_StrengthFar
} HscIndentBlur_;

// The avaliable fields of a snapshot.
typedef struct {
  HscIndentAxis pos;
  i08 rot;

  i08 fov;
  i08 gamma;
  HscIndentBlur blur;
} HscSnapshotIntent;

// Camera status snapshot, for interpolation.
typedef struct {
  // Rotation and position.
  union {
    struct {
      v4f left;
      v4f up;
      v4f backward;
      v4f pos;
    };
    m44 matrix;
  };

  // Blur.
  union {
    struct {
      f32 distNear;
      f32 distFar;
      f32 strengthNear;
      f32 strengthFar;
    };
    v4f blur;
  };

  f32 fov;
  f32 gamma;

  // Related object.
  HscSnapshotIntent *intent;
} HscSnapshot;

typedef struct {
  i08 isMenuShown;

  // -- General controller.
  // Deciding whether to take over the camera.
  i08 enable;
  // Deciding whether to display the camera UI.
  i08 noOriginalUi;
  // Deciding on how to take over camera operations.
  HscMajorMode majorMode;
  // Deciding which camera mode to take over.
  HscCameraMode cameraMode;
  // Deciding whether to reset camera coordinates for the next frame.
  i08 resetPosFlag;

  // -- Keyboard and mouse inputs.
  v4f movementInput;
  v4f facingInput;

  // -- Camera parameters from user input and for display with ImGui.
  // Position.
  v4f pos;
  // Rotation in radians, for internal calculations only.
  v4f rot;
  // Rotation in degrees, for user inputs.
  v4f rotDeg;
  // Adjustable parameters when using SkyCamera.
  f32 scale;
  f32 focus;
  f32 brightness;

  // -- Camera static parameter override flags.
  i08 overridePos;
  i08 overrideDir;
  i08 overrideFocus;
  i08 overrideScale;
  i08 overrideBrightness;

  // -- Freecam data.
  f32 freecamSpeed;
  f32 freecamRotateSpeed;
  i08 freecamCollision;
  i08 freecamLockPosition;
  i08 freecamLockRotation;

  // -- FPV data.
  i32 fpvMode;
} HscContext;

// Key bindings.
typedef union {
  struct {
    // -- Menu toggle.
    HTHandle menu;

    // -- Movement inputs.
    HTHandle foward;
    HTHandle backward;
    HTHandle left;
    HTHandle right;
    HTHandle up;
    HTHandle down;
    HTHandle rollLeft;
    HTHandle rollRight;

    // -- Dynamic mode simulation operations.
    HTHandle resetPos;
    HTHandle pause;
  };
  HTHandle keys[11];
} HscKeyBindings;

typedef struct {
  struct {
    f32 mouseSensitivity;
    f32 verticalSenseScale;
    i08 swapRollYaw;
  } control;
} HscOptions;

extern HMODULE hModuleDll;
extern HscKeyBindings gBindedKeys;
// Global context of the plugin.
extern HscContext gContext;
extern HscOptions gOptions;
extern v4f gMouseDeltaPx;

// The camera status to be updated to the current camera object of the game.
extern HscSnapshot gCamera;
// The avaliable fields in gCamera.
extern HscSnapshotIntent gCameraIntent;

// Validate the matrix of the snapshot.
static inline void hscValidateSnapshot(
  HscSnapshot *snapshot
) {
  snapshot->matrix._14 = snapshot->matrix._24 = snapshot->matrix._34 = 0;
  snapshot->matrix._44 = 1.0f;
}

static inline void hscSnapshotSetPos(
  HscSnapshot *snapshot,
  v4f pos
) {
  snapshot->pos = pos;
  hscValidateSnapshot(snapshot);
}

//-----------------------------------------------------------------------------
// [SECTION] HSC_LOGGER
//-----------------------------------------------------------------------------

static void hscLog(
  const char *format,
  ...
) {
  va_list arg;

  va_start(arg, format);
  HTTellTextV(format, arg);
  va_end(arg);
}

#define LOGI(format, ...) hscLog("§f[hSC][INFO] " format, ##__VA_ARGS__)
#define LOGW(format, ...) hscLog("§e[hSC][WARN] " format, ##__VA_ARGS__)
#define LOGE(format, ...) hscLog("§c[hSC][ERR] " format, ##__VA_ARGS__)
#define LOGEF(format, ...) hscLog("§c[hSC][ERR][FATAL] " format, ##__VA_ARGS__)

//-----------------------------------------------------------------------------
// [SECTION] HSC_SETUP
//-----------------------------------------------------------------------------

#define REQUIRED_FUNC_COUNT 9

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
} FuncAddresses;

typedef struct {
  HTAsmSig s;
  const char *name;
} NamedSig;

extern const NamedSig *const RequiredFn[REQUIRED_FUNC_COUNT];

i08 hscSetupFuncWithSig(
  FuncAddresses *functions);

//-----------------------------------------------------------------------------
// [SECTION] HSC_HOOK
//-----------------------------------------------------------------------------

// Typedefs of trampoline functions.
typedef u64 (__fastcall *PFN_SkyCameraProp_update)(SkyCameraProp *, u64);
typedef u64 (__fastcall *PFN_SkyCameraProp__updateParams)(SkyCameraProp *, u64);
typedef u64 (__fastcall *PFN_SkyCameraProp_updateUI)(
  SkyCameraProp *, u64, u64, u64, f32 *, f32 *, f32 *, u64, i08);
typedef u64 (__fastcall *PFN_World_interactionTest)(
  u64, v4f *, v4f *, float, v4f *, i08 *);
typedef u64 (__fastcall *PFN_WhiskerCamera_update)(
  WhiskerCamera *, u64 *);
typedef u64 (__fastcall *PFN_SkyCamera_update)(
  SkyCamera *, u64 *);
typedef u64 (__fastcall *PFN_MainCamera__getDelta)(
  u64, i08 *, u64, u64, u64, int, int);
typedef u64 (__fastcall *PFN_Input_getMouseDeltaPx)(
  u64, v4f *);

extern u64 gSavedLevelContext;
extern FuncAddresses gTramp;

i08 hscInitAllHooks();
i08 hscCreateAllHooks();
i08 hscRemoveAllHooks();

//-----------------------------------------------------------------------------
// [SECTION] HSC_CAM_UPDATE
//-----------------------------------------------------------------------------

void hscPreupdateStatic(
  MainCamera *);
void hscPreupdateDynamic(
  MainCamera *);
void hscPreupdateAnim(
  MainCamera *);

void hscUpdatePropMain(
  SkyCameraProp *);
void hscPreupdateCameraMain(
  MainCamera *);
void hscUpdateCameraMain(
  MainCamera *);

//-----------------------------------------------------------------------------
// [SECTION] HSC_UI
//-----------------------------------------------------------------------------

// ImGui widgets.
void hscUIShowTips(
  const char *desc,
  i08 sameLine);

// Submenus.
void hscUIMenuStatic();
void hscUIMenuDynamic();
void hscUIMenuAnim();
void hscUIMenuSettings();

// Windows.
void hscUIWindowMain();

// Handle keyboard and mouse inputs.
void hscInputHandler();
// Key event callback.
void hscKeyEventCallbackMain(
  HTKeyEvent *);
// Convert mouse movement to radians.
v4f hscGetFacingDeltaRad();

#ifdef __cplusplus
}
#endif

#endif
