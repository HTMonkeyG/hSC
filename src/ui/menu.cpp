#include <math.h>
#include "imgui.h"

#include "mth/macros.h"
#include "internal.h"

static const char *FPVMODES[] = { "Elytra", "Barrel-roll" };
static const f32 STEP = 0.1f;

void hscUIShowTips(
  const char *desc,
  i08 sameLine
) {
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
static void inputFloatXYZ(f32 *v, f32 cursorX) {
  ImGui::PushID((void *)v);
  InputSingleAxis("X", "##X", v, cursorX);
  InputSingleAxis("Y", "##Y", v + 1, cursorX);
  InputSingleAxis("Z", "##Z", v + 2, cursorX);
  ImGui::PopID();
}
#undef InputSingleAxis

void hscUIMenuStatic() {
  ImGui::SeparatorText("Set Camera Params");

  // Camera position input.
  ImGui::Checkbox("Pos", (bool *)&gContext.overridePos);
  ImGui::SameLine();
  inputFloatXYZ(&gContext.pos.x, ImGui::GetCursorPosX());

  // Camera rotation input.
  ImGui::Checkbox("Rot", (bool *)&gContext.overrideDir);
  ImGui::SameLine();
  inputFloatXYZ(&gContext.rotDeg.x, ImGui::GetCursorPosX());

  // Clamp camera facing.
  gContext.rotDeg.x = fmodf(gContext.rotDeg.x, 360.0f);
  gContext.rotDeg.z = fmodf(gContext.rotDeg.z, 360.0f);
  gContext.rotDeg.y = m_clamp(gContext.rotDeg.y, -89.75f, 89.75f);

  ImGui::BeginDisabled(gContext.cameraMode == HscCameraMode_Whisker);

  // Camera scale input.
  ImGui::Checkbox("##cb3", (bool *)&gContext.overrideScale);
  ImGui::SameLine();
  ImGui::DragFloat("Scale", &gContext.scale, .01f, 0.0f, 1.0f);

  // Camera focus(blur) input.
  ImGui::Checkbox("##cb4", (bool *)&gContext.overrideFocus);
  ImGui::SameLine();
  ImGui::DragFloat("Focus", &gContext.focus, .01f, 0.0f, 1.0f);

  // Camera brightness input.
  ImGui::Checkbox("##cb5", (bool *)&gContext.overrideBrightness);
  ImGui::SameLine();
  ImGui::DragFloat("Brightness", &gContext.brightness, .01f, 0.0f, 1.0f);

  ImGui::EndDisabled();

  ImGui::SeparatorText("Freecam params");

  if (ImGui::Button("Reset pos") || HTHotkeyPressed(gBindedKeys.resetPos))
    gContext.resetPosFlag = 1;

  ImGui::Checkbox("Disable camera movement", (bool *)&gContext.freecamLockPosition);
  hscUIShowTips(
    "When this item is checked, the movement inputs will pass to the game.",
    1);
  ImGui::Checkbox("Disable camera rotation", (bool *)&gContext.freecamLockRotation);
  hscUIShowTips(
    "When this item is checked, hSC will ignore mouse inputs.",
    1);
  ImGui::Checkbox("Check collision", (bool *)&gContext.freecamCollision);
  ImGui::DragFloat("Rotate speed", &gContext.freecamRotateSpeed, .01f, 0, 10.0f);
  ImGui::DragFloat("Movement speed", &gContext.freecamSpeed, .01f, 0, 100.0f);
}

void hscUIMenuDynamic() {
  ImGui::SeparatorText("Dynamic mode");

  ImGui::Combo(
    "Mode",
    &gContext.fpvMode,
    FPVMODES,
    IM_ARRAYSIZE(FPVMODES));

  if (ImGui::Button("Reset pos") || HTHotkeyPressed(gBindedKeys.resetPos))
    gContext.resetPosFlag = 1;
}

void hscUIMenuAnim() {
  ImGui::SeparatorText("Animation mode");
  // TODO: Under development.
}

void hscUIMenuSettings() {
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
  hscUIShowTips(
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
