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

static void gui_subMenuSet() {
  ImGui::SeparatorText("Set Camera Params");

  // Camera position input.
  ImGui::Checkbox("##cb1", (bool *)&gState.overridePos);
  ImGui::SameLine();
  ImGui::InputFloat3("Pos", (float *)&gState.pos);

  // Camera rotation input.
  ImGui::Checkbox("##cb2", (bool *)&gState.overrideDir);
  ImGui::SameLine();
  ImGui::InputFloat3("Rotation", (float *)&gState.rot);

  // Clamp camera facing.
  gState.rot.x = fmodf(gState.rot.x, 360.0f);
  gState.rot.z = fmodf(gState.rot.z, 360.0f);
  gState.rot.y = m_clamp(gState.rot.y, -89.75f, 89.75f);

  ImGui::BeginDisabled(gState.cameraMode == 3);

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
}

static void gui_subMenuStatic() {
  ImGui::SeparatorText("Set Camera Params");

  // Camera position input.
  ImGui::Checkbox("Position", (bool *)&gState.overridePos);
  ImGui::InputScalarN("##PosInputs", ImGuiDataType_Float, (float *)&gState.pos, 3, &STEP, nullptr, "%.3f");

  // Camera rotation input.
  ImGui::Checkbox("Rotation", (bool *)&gState.overrideDir);

  ImGui::Text("X");
  ImGui::SameLine();
  ImGui::InputScalar("##RotInputX", ImGuiDataType_Float, &gState.rot.x, &STEP, nullptr, "%.3f");
  ImGui::Text("Y");
  ImGui::SameLine();
  ImGui::InputScalar("##RotInputY", ImGuiDataType_Float, &gState.rot.y, &STEP, nullptr, "%.3f");
  ImGui::Text("Z");
  ImGui::SameLine();
  ImGui::InputScalar("##RotInputZ", ImGuiDataType_Float, &gState.rot.z, &STEP, nullptr, "%.3f");

  // Clamp camera facing.
  gState.rot.x = fmodf(gState.rot.x, 360.0f);
  gState.rot.z = fmodf(gState.rot.z, 360.0f);
  gState.rot.y = m_clamp(gState.rot.y, -89.75f, 89.75f);

  ImGui::BeginDisabled(gState.cameraMode == 3);

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

  ImGui::Checkbox("Check collision", (bool *)&gState.freecamCollision);
  ImGui::DragFloat("Rotate Speed", &gState.freecamRotateSpeed, .01f, 0, 10.0f);
  ImGui::DragFloat("Speed", &gState.freecamSpeed, .01f, 0, 100.0f);
  if (ImGui::Button("Reset pos"))
    gState.resetPosFlag = 1;
}

static void gui_subMenuFreecam() {
  ImGui::SeparatorText("Free camera");

  ImGui::Combo(
    "Mode",
    &gState.freecamMode,
    FREECAMMODES,
    IM_ARRAYSIZE(FREECAMMODES));

  ImGui::Checkbox("Check collision", (bool *)&gState.freecamCollision);

  ImGui::DragFloat("Rotate Speed", &gState.freecamRotateSpeed, .01f, 0, 10.0f);

  ImGui::DragFloat("Speed", &gState.freecamSpeed, .01f, 0, 100.0f);
  if (ImGui::Button("Reset pos"))
    gState.resetPosFlag = 1;
}

static void gui_subMenuFPV() {
  ImGui::SeparatorText("FPV");

  ImGui::Combo(
    "Mode",
    &gState.fpvMode,
    FPVMODES,
    IM_ARRAYSIZE(FPVMODES));

  if (ImGui::Button("Reset pos"))
    gState.resetPosFlag = 1;
}

static void gui_subMenuDynamic() {
  ImGui::SeparatorText("FPV");

  ImGui::Combo(
    "Mode",
    &gState.fpvMode,
    FPVMODES,
    IM_ARRAYSIZE(FPVMODES));

  if (ImGui::Button("Reset pos"))
    gState.resetPosFlag = 1;
}

static void gui_subMenuAnim() {
  ImGui::SeparatorText("FPV");

  ImGui::Combo(
    "Mode",
    &gState.fpvMode,
    FPVMODES,
    IM_ARRAYSIZE(FPVMODES));

  if (ImGui::Button("Reset pos"))
    gState.resetPosFlag = 1;
}

static void gui_navMain() {
  if (ImGui::BeginMenuBar()) {
    if (ImGui::BeginMenu("Edit")) {
      ImGui::MenuItem("Preferences", nullptr, (bool *)&gGui.showSettings);
      ImGui::EndMenu();
    }
    ImGui::EndMenuBar();
  }
}

void gui_windowMain() {
  ImGuiIO &io = ImGui::GetIO();
  (void)io;

  // Title.
  if (!ImGui::Begin(
    "hSC Main",
    (bool *)&gGui.isOpen,
    ImGuiWindowFlags_MenuBar
  )) {
    ImGui::End();
    return;
  }

  gui_navMain();

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
      ImGui::Text("Static mode.");
      gui_subMenuStatic();
      ImGui::EndTabItem();
    }
    if (ImGui::BeginTabItem("Dynamic")) {
      ImGui::Text("Dynamic mode.");
      gui_subMenuDynamic();
      ImGui::EndTabItem();
    }
    if (ImGui::BeginTabItem("Animation")) {
      ImGui::Text("Animation mode.");
      gui_subMenuAnim();
      ImGui::EndTabItem();
    }
    if (ImGui::BeginTabItem("Settings")) {
      ImGui::Text("Settings.");
      gui_subMenuSet();
      ImGui::EndTabItem();
    }
    if (ImGui::BeginTabItem("Abouts")) {
      ImGui::SeparatorText("Abouts.");
      ImGui::Text("hSC Plugin v0.1.2 by HTMonkeyG");
      ImGui::Text("A camera plugin developed for Sky:CotL.");
      ImGui::Text("<https://www.github.com/HTMonkeyG/hSC>");
      ImGui::EndTabItem();
    }
    ImGui::EndTabBar();
  }

  ImGui::RadioButton("Set", &gState.majorMode, 0);
  ImGui::SameLine();
  ImGui::RadioButton("Freecam", &gState.majorMode, 1);
  ImGui::SameLine();
  ImGui::RadioButton("FPV", &gState.majorMode, 2);

  if (gState.majorMode == 0)
    gui_subMenuSet();
  if (gState.majorMode == 1)
    ;//gui_subMenuFreecam();
  if (gState.majorMode == 2)
    gui_subMenuFPV();

  // Overlay window FPS display.
  ImGui::Text("Overlay %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);

  ImGui::End();
}
