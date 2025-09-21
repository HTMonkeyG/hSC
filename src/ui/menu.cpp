#include <math.h>
#include "imgui.h"

#include "ui/ui.h"
#include "mth/macros.h"

static const char *MODES[] = { "FirstPerson", "Front", "Placed", "WhiskerCamera" }
  , *FREECAMMODES[] = { "Orientation", "Axial", "Full-direction" }
  , *FPVMODES[] = { "Elytra", "Barrel-roll" };
static const f32 STEP = 0.1f;

void gui_displayTips(const char *desc, i08 sameLine) {
  if (sameLine)
    ImGui::SameLine();
  ImGui::TextDisabled("(?)");
  if (ImGui::BeginItemTooltip()) {
    ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
    ImGui::TextUnformatted(desc);
    ImGui::PopTextWrapPos();
    ImGui::EndTooltip();
  }
}

#define InputSingleAxis(n, l, p, c) (void)(\
  ImGui::SetCursorPosX(c),\
  ImGui::AlignTextToFramePadding(),\
  ImGui::Text(n),\
  ImGui::SameLine(),\
  ImGui::InputScalar(l, ImGuiDataType_Float, p, &STEP, nullptr, "%.3f")\
)
static void gui_inputXYZ(f32 *v, f32 cursorX) {
  ImGui::PushID((void *)v);
  InputSingleAxis("X", "##X", v, cursorX);
  InputSingleAxis("Y", "##Y", v + 1, cursorX);
  InputSingleAxis("Z", "##Z", v + 2, cursorX);
  ImGui::PopID();
}
#undef InputSingleAxis

static void gui_subMenuStatic() {
  ImGui::SeparatorText("Set Camera Params");

  // Camera position input.
  ImGui::Checkbox("Pos", (bool *)&gState.overridePos);
  ImGui::SameLine();
  gui_inputXYZ(&gState.pos.x, ImGui::GetCursorPosX());

  // Camera rotation input.
  ImGui::Checkbox("Rot", (bool *)&gState.overrideDir);
  ImGui::SameLine();
  gui_inputXYZ(&gState.rotDeg.x, ImGui::GetCursorPosX());

  // Clamp camera facing.
  gState.rotDeg.x = fmodf(gState.rotDeg.x, 360.0f);
  gState.rotDeg.z = fmodf(gState.rotDeg.z, 360.0f);
  gState.rotDeg.y = m_clamp(gState.rotDeg.y, -89.75f, 89.75f);

  ImGui::BeginDisabled(gState.cameraMode == CM_WHISKER);

  // Camera scale input.
  ImGui::Checkbox("##cb3", (bool *)&gState.overrideScale);
  ImGui::SameLine();
  ImGui::DragFloat("Scale", &gState.scale, .01f, 0.0f, 1.0f);

  // Camera focus(blur) input.
  ImGui::Checkbox("##cb4", (bool *)&gState.overrideFocus);
  ImGui::SameLine();
  ImGui::DragFloat("Focus", &gState.focus, .01f, 0.0f, 1.0f);

  // Camera brightness input.
  ImGui::Checkbox("##cb5", (bool *)&gState.overrideBrightness);
  ImGui::SameLine();
  ImGui::DragFloat("Brightness", &gState.brightness, .01f, 0.0f, 1.0f);

  ImGui::EndDisabled();

  ImGui::SeparatorText("Freecam params");

  if (ImGui::Button("Reset pos"))
    gState.resetPosFlag = 1;

  ImGui::Checkbox("Lock position", (bool *)&gState.freecamLockPosition);
  ImGui::Checkbox("Lock rotation", (bool *)&gState.freecamLockRotation);
  ImGui::Checkbox("Check collision", (bool *)&gState.freecamCollision);
  ImGui::DragFloat("Rotate speed", &gState.freecamRotateSpeed, .01f, 0, 10.0f);
  ImGui::DragFloat("Movement speed", &gState.freecamSpeed, .01f, 0, 100.0f);
}

static void gui_subMenuDynamic() {
  ImGui::SeparatorText("Dynamic mode");

  ImGui::Combo(
    "Mode",
    &gState.fpvMode,
    FPVMODES,
    IM_ARRAYSIZE(FPVMODES));

  if (ImGui::Button("Reset pos"))
    gState.resetPosFlag = 1;
}

static void gui_subMenuAnim() {
  ImGui::SeparatorText("Animation mode");
  // TODO: Under development.
}

static void gui_subMenuSettings() {
  ImGui::SeparatorText("General settings");

  ImGui::Text("Mouse sensitivity");
  ImGui::DragFloat(
    "##settingsDrag1",
    &gOptions.general.mouseSensitivity,
    0.01f,
    0.0f,
    10.0f);
  ImGui::Text("Vertical sensitivity scale");
  ImGui::DragFloat(
    "##settingsDrag2",
    &gOptions.general.verticalSenseScale,
    0.01f,
    0.0f,
    2.0f);

  ImGui::SeparatorText("Freecam settings");
  
  ImGui::Text("Full-takeover by default");
  gui_displayTips(
    "The plugin will automatically obtain mouse increments and calculate"
    "rotations, instead of using the game's original calculations.",
    1);
  ImGui::Checkbox(
    "##settingsCheckbox3",
    (bool *)&gOptions.freecam.fullTakeover);

  ImGui::Text("Swap yaw and roll");
  ImGui::Checkbox(
    "##settingsCheckbox4",
    (bool *)&gOptions.freecam.swapRollYaw);
}

void gui_windowMain() {
  ImGuiIO &io = ImGui::GetIO();
  (void)io;

  // Title.
  if (!ImGui::Begin("hSC Main", (bool *)&gGui.isOpen))
    return (void)ImGui::End();
  
  // Overlay window FPS display.
  ImGui::Text("Overlay %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);

  // General options.
  if (ImGui::Checkbox("Take over", (bool *)&gState.enable))
    gState.resetPosFlag = 1;
  ImGui::Combo("Use mode", &gState.cameraMode, MODES, IM_ARRAYSIZE(MODES));
  ImGui::Checkbox("No UI", (bool *)&gState.noOriginalUi);
  gui_displayTips(
    "Hide the original camera UI. Please adjust the parameters before select"
    "this item.",
    1);
  
  if (ImGui::BeginTabBar("Mode select")) {
    if (ImGui::BeginTabItem("Static")) {
      gState.majorMode = MM_STATIC;
      gui_subMenuStatic();
      ImGui::EndTabItem();
    }
    if (ImGui::BeginTabItem("Dynamic")) {
      gState.majorMode = MM_DYNAMIC;
      gui_subMenuDynamic();
      ImGui::EndTabItem();
    }
    if (ImGui::BeginTabItem("Animation")) {
      gState.majorMode = MM_ANIMATION;
      gui_subMenuAnim();
      ImGui::EndTabItem();
    }
    if (ImGui::BeginTabItem("Settings")) {
      ImGui::Text("Settings.");
      gui_subMenuSettings();
      ImGui::EndTabItem();
    }
    if (ImGui::BeginTabItem("Abouts")) {
      ImGui::SeparatorText("Abouts.");
      ImGui::Text("hSC Plugin v" HSC_VERSION " by HTMonkeyG");
      ImGui::Text("A camera plugin developed for Sky:CotL.");
      ImGui::Text("<https://www.github.com/HTMonkeyG/hSC>");
      ImGui::EndTabItem();
    }
    ImGui::EndTabBar();
  }

  ImGui::End();
}
