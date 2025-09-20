#include <stdarg.h>
#include <stdio.h>
#include "imgui.h"

#include "internal.h"
#include "ui/ui.h"

static ImVector<char *> gLines;

static void gui_addConsoleLine(const char* fmt, ...) {
  char buf[1024];
  va_list args;
  size_t len;
  void *mem;

  va_start(args, fmt);
  vsnprintf(buf, IM_ARRAYSIZE(buf), fmt, args);
  buf[IM_ARRAYSIZE(buf) - 1] = 0;
  va_end(args);

  len = strlen(buf) + 1;
  mem = ImGui::MemAlloc(len);
  gLines.push_back((char *)memcpy(mem, (const void*)buf, len));
}

static void gui_clearConsole() {
  for (int i = 0; i < gLines.Size; i++)
    ImGui::MemFree(gLines[i]);
  gLines.clear();
}

void gui_windowConsole() {
  ImGuiListClipper clipper;
  f32 height;
  char input[1024];
  bool focus = false;

  ImGui::Begin("hSC Console");

  height = ImGui::GetStyle().ItemSpacing.y + ImGui::GetFrameHeightWithSpacing();
  if (ImGui::BeginChild(
    "ConsoleLog",
    ImVec2(0, -height),
    ImGuiChildFlags_NavFlattened,
    ImGuiWindowFlags_HorizontalScrollbar
  )) {
    if (ImGui::BeginPopupContextWindow()) {
      if (ImGui::Selectable("Clear console"))
        gui_clearConsole();
      ImGui::EndPopup();
    }

    clipper.Begin(gLines.Size);
    while (clipper.Step())
      for (int i = clipper.DisplayStart; i < clipper.DisplayEnd; i++) {
        ImGui::TextUnformatted(gLines[i]);
      }
  }
  ImGui::EndChild();
  ImGui::Separator();

  if (ImGui::InputText(
    "Input",
    input,
    IM_ARRAYSIZE(input),
    ImGuiInputTextFlags_EnterReturnsTrue
      | ImGuiInputTextFlags_EscapeClearsAll
      | ImGuiInputTextFlags_CallbackCompletion
      | ImGuiInputTextFlags_CallbackHistory
  )) {
    // 
    focus = true;
  }

  ImGui::SetItemDefaultFocus();
  if (focus)
    ImGui::SetKeyboardFocusHere(-1);

  ImGui::End();
}
