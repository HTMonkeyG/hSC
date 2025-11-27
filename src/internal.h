#ifndef __INTERNAL_H__
#define __INTERNAL_H__

#include <windows.h>
#include "includes/htmodloader.h"

#include "aliases.h"
#include "mth/macros.h"
#include "mth/vector.h"
#include "mth/matrix.h"
#include "mth/euler.h"
#include "mth/aabb.h"

#define HSC_VERSION "0.4.0"

#if HTML_VERSION < 10801
#error "HTML SDK version must >= v1.8.1"
#endif

#ifdef __cplusplus
extern "C" {
#endif

//-----------------------------------------------------------------------------
// [SECTION] SKY_GAME_TYPES
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
  union {
    struct {
      // Rotate matrix (3x3) is stored separately by rows in three vectors. The
      // last component of these vectors is 0.
      v4f left;
      v4f up;
      v4f backward;
      // The last element of this vector is 1. We can consider the four vectors
      // as a single matrix (4x4).
      v4f cameraPos;
    };
    m44 mat;
  };
  char unk_4[32];
} MainCameraContext;

// The parent class of all possible camera updates.
typedef struct MainCamera_ {
  u64 *lpVtbl;
  u64 unk_1;
  u64 unk_2;
  struct MainCamera_ *prev;
  struct MainCamera_ *next;
  u64 unk_3;
  MainCameraContext context1;
  MainCameraContext context2;
  u64 active;
} MainCamera;

// The size of the following two structs is incorrect. This definition only
// contains components what we need.

// The states of the default camera of the player.
typedef struct {
  MainCamera super;
  char unk_1[56];
  v4f mouseDelta;
  char unk_2[1456];
} WhiskerCamera;

// The camera status of the players's camera accessory.
typedef struct {
  MainCamera super;
  void *prop;
  void *unk_2;
  void *unk_3;
  v4f pos;
  v4f dir;
} SkyCamera;

#define SkyCamera_getPropPtr(sc) ((SkyCameraProp *)((i08 *)(((SkyCamera *)(sc))->prop) - 0x88))

// The return value of World::interactionTest().
typedef struct {
  // Intersection point.
  v4f intersection;
  // Normal vector.
  v4f normal;
  i08 unk[0x40];
} InteractionResult;

//-----------------------------------------------------------------------------
// [SECTION] HSC_GLOBALS
//-----------------------------------------------------------------------------

// The avaliable fields in a vector.
typedef i32 HscIndentAxis;
typedef enum {
  HscIndentAxis_None = 0,

  HscIndentAxis_X = 1 << 0,
  HscIndentAxis_Y = 1 << 1,
  HscIndentAxis_Z = 1 << 2,
  HscIndentAxis_W = 1 << 3,

  HscIndentAxis_All = 0x0F
} HscIndentAxis_;

// The avaliable fields of a snapshot.
typedef struct {
  HscIndentAxis pos;
  i08 rot;

  i08 fov;
  i08 gamma;
  i08 blur;
} HscSnapshotIntent;

// Camera status snapshot, for interpolation.
typedef struct {
  union {
    struct {
      v4f left;
      v4f up;
      v4f backward;
      v4f pos;
    };
    m44 matrix;
  };

  f32 fov;
  f32 gamma;
  f32 blur;

  HscSnapshotIntent *intent;
} HscSnapshot;

typedef struct {
  // -- General controller.
  // Deciding whether to take over the camera.
  i08 enable;
  // Deciding whether to display the camera UI.
  i08 noOriginalUi;
  // Deciding on how to take over camera operations.
  i32 majorMode;
  // Deciding which camera mode to take over.
  i32 cameraMode;
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
    HTHandle menu;
    HTHandle foward;
    HTHandle backward;
    HTHandle left;
    HTHandle right;
    HTHandle up;
    HTHandle down;
    HTHandle rollLeft;
    HTHandle rollRight;
  };
  HTHandle keys[9];
} HscKeyBindings;

extern HMODULE hModuleDll;
extern HscKeyBindings gBindedKeys;
extern HscContext gContext;
extern HscSnapshot gCamera;
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

typedef struct {
  MainCamera super;

  i08 useFov;
  i08 useMatrix;
  i08 usePos;
} InjectCamera;

// Unused. Inject our camera in the game's camera system.
extern InjectCamera gInjectCamera;

void hscPreupdateStatic(MainCamera *);
void hscPreupdateDynamic(MainCamera *);
void hscPreupdateAnim(MainCamera *);

void hscUpdatePropMain(SkyCameraProp *);
void hscPreupdateCameraMain(MainCamera *);
void hscUpdateCameraMain(MainCamera *);

#ifdef __cplusplus
}
#endif

#endif

