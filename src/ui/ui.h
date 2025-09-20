#ifndef __UI_H__
#define __UI_H__

#include "internal.h"

#define WM_USER_EXIT (0x8000 + 1)

#define MM_STATIC (0x00)
#define MM_DYNAMIC (0x01)
#define MM_ANIMATION (0x02)

#define OM_SET (0x00)
#define OM_FREE (0x01)
#define OM_FPV (0x02)
#define OM_WHISKER (0x03)

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
  i08 enable;
  i08 noOriginalUi;
  i32 majorMode;
  i32 cameraMode;
  i08 resetPosFlag;

  // Inputs.
  v4f movementInput;
  v4f facingInput;

  // Original data sync.
  v4f pos;
  v4f rot;
  f32 scale;
  f32 focus;
  f32 brightness;

  // Set param mode.
  i08 overridePos;
  i08 overrideDir;
  i08 overrideFocus;
  i08 overrideScale;
  i08 overrideBrightness;

  // Freecam mode.
  i32 freecamMode;
  f32 freecamSpeed;
  f32 freecamRotateSpeed;
  i08 freecamCollision;

  // FPV mode.
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

i08 gui_init(HMODULE hModule);
i08 gui_deinit();
i08 gui_waitForInit();
i08 gui_waitForDll(DWORD *lastError);
i08 gui_update();

extern v4f gMouseDeltaPx;

/**
 * Handle keyboard and mouse inputs.
 */
void gui_handleInput();

/**
 * Convert mouse movement to radians.
 */
v4f gui_getFacingDeltaRad();

void gui_displayTips(const char *desc, i08 sameLine);
void gui_windowMain();

void gui_windowSettings();

void gui_windowConsole();

#ifdef __cplusplus
}
#endif

#endif
