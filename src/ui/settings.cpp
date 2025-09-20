#include "imgui.h"

#include "ui/ui.h"

void gui_windowSettings() {
  ImGuiIO &io = ImGui::GetIO();
  (void)io;

  if (!ImGui::Begin(
    "hSC Settings",
    (bool *)&gGui.showSettings
  )) {
    ImGui::End();
    return;
  }

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

  ImGui::End();
}
