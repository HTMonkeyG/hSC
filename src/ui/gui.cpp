#include <windows.h>
#include "imgui.h"

#include "internal.h"

static const char *MODES[] = {
  "FirstPerson",
  "Selfie",
  "Tripod",
  "WhiskerCamera"
};

LARGE_INTEGER gPerformFreq;
LARGE_INTEGER gLastFrame;
f32 gTimeElapsed;
HscContext gContext = {0};
HscOptions gOptions = {
  .general = {
    .mouseSensitivity = 3.35f,
    .verticalSenseScale = 1.0f
  },
  .freecam = {
    .swapRollYaw = 0
  }
};

void hscUIWindowMain() {
  ImGuiIO &io = ImGui::GetIO();
  (void)io;

  // Title.
  if (!ImGui::Begin("hSC Main", (bool *)&gContext.isMenuShown))
    return (void)ImGui::End();
  
  // Overlay window FPS display.
  ImGui::Text("Overlay %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);

  // General options.
  if (ImGui::Checkbox("Enable plugin", (bool *)&gContext.enable))
    ;//gContext.resetPosFlag = 1;
  ImGui::Combo("Mode overriden", &gContext.cameraMode, MODES, IM_ARRAYSIZE(MODES));
  ImGui::Checkbox("No UI", (bool *)&gContext.noOriginalUi);
  hscUIShowTips(
    "Hide the original camera UI. Please adjust the parameters before select"
    "this item.",
    1);

  if (ImGui::BeginTabBar("Mode select")) {
    if (ImGui::BeginTabItem("Static")) {
      gContext.majorMode = HscMajorMode_Static;
      hscUIMenuStatic();
      ImGui::EndTabItem();
    }
    if (ImGui::BeginTabItem("Dynamic")) {
      gContext.majorMode = HscMajorMode_Dynamic;
      hscUIMenuDynamic();
      ImGui::EndTabItem();
    }
    if (ImGui::BeginTabItem("Animation")) {
      gContext.majorMode = HscMajorMode_Animation;
      hscUIMenuAnim();
      ImGui::EndTabItem();
    }
    if (ImGui::BeginTabItem("Settings")) {
      hscUIMenuSettings();
      ImGui::EndTabItem();
    }
    if (ImGui::BeginTabItem("Abouts")) {
      ImGui::Text("hSC Plugin v" HSC_VERSION " by HTMonkeyG");
      ImGui::Text("A camera plugin developed for Sky:CotL.");
#ifdef HTML_VERSION_NAME
      ImGui::Text("Compiled on HTModLoader SDK v" HTML_VERSION_NAME ".");
#endif
      ImGui::TextLinkOpenURL(
        "<https://www.github.com/HTMonkeyG/hSC>",
        "https://www.github.com/HTMonkeyG/hSC");
      ImGui::EndTabItem();
    }
    ImGui::EndTabBar();
  }

  ImGui::End();
}
