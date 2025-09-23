#include "ui/ui.h"

GUI_t gGui = {0};
GUIState_t gState = {0};
GUIOptions_t gOptions = {
  .general = {
    .mouseSensitivity = 3.35f,
    .verticalSenseScale = 1.0f
  },
  .freecam = {
    .swapRollYaw = 0
  }
};
