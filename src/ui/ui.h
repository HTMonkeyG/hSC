#ifndef __UI_H__
#define __UI_H__

#include "internal.h"

#define WM_USER_EXIT (0x8000 + 1)

#define MM_STATIC (0x00)
#define MM_DYNAMIC (0x01)
#define MM_ANIMATION (0x02)

#define FC_ORIENT (0x00)
#define FC_AXIAL (0x01)
#define FC_FULLDIR (0x02)

#define CM_FIRST (0x00)
#define CM_FRONT (0x01)
#define CM_PLACE (0x02)
#define CM_WHISKER (0x03)

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
  // General controller.
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

  // Inputs.
  v4f movementInput;
  v4f facingInput;

  // Camera parameters.
  // Position.
  v4f pos;
  // Rotation in radians, for internal calculations.
  v4f rot;
  // Rotation in degrees, only for user operation.
  v4f rotDeg;
  // Adjustable parameters during normal gaming, not avaliable on OM_WHISKER.
  f32 scale;
  f32 focus;
  f32 brightness;

  // Camera static parameters override.
  i08 overridePos;
  i08 overrideDir;
  i08 overrideFocus;
  i08 overrideScale;
  i08 overrideBrightness;

  // Freecam datas.
  i32 freecamMode;
  f32 freecamSpeed;
  f32 freecamRotateSpeed;
  i08 freecamCollision;
  i08 freecamLockPosition;
  i08 freecamLockRotation;

  // FPV datas.
  i32 fpvMode;

  // Pre-calculated rotation matrix and pos.
  i08 useMatrix;
  i08 usePos;
  v4f mat[4];
} GUIState_t;

typedef struct {
  HWND hWnd;
  HANDLE hInit;
  i64 performFreq;
  i64 lastFrameCounter;
  f32 timeElapsedSecond;
  f32 dpiScale;
  i08 isOpen;
  i08 showSettings;
} GUI_t;

typedef struct {
  struct {
    f32 mouseSensitivity;
    f32 verticalSenseScale;
  } general;
  struct {
    i08 swapRollYaw;
    i08 fullTakeover;
  } freecam;
} GUIOptions_t;

extern GUI_t gGui;
extern GUIState_t gState;
extern GUIOptions_t gOptions;
extern v4f gMouseDeltaPx;

static inline void setRotDegree(v4f rotRadians) {
  gState.rotDeg = v4fscale(rotRadians, 180.0f / PI_F);
  gState.rotDeg.x = fmodf(gState.rotDeg.x, 360.0f);
  gState.rotDeg.z = fmodf(gState.rotDeg.z, 360.0f);
  gState.rotDeg.y = m_clamp(gState.rotDeg.y, -89.75f, 89.75f);
}

i08 gui_init(HMODULE hModule);
i08 gui_deinit();
i08 gui_waitForInit();
i08 gui_waitForDll(DWORD *lastError);
i08 gui_update();

void gui_displayTips(const char *desc, i08 sameLine);
void gui_windowMain();
void gui_windowConsole();

/**
 * Handle keyboard and mouse inputs.
 */
void gui_handleInput();
/**
 * Convert mouse movement to radians.
 */
v4f gui_getFacingDeltaRad();

void gui_initInputs();

#ifdef __cplusplus
}
#endif

#endif
