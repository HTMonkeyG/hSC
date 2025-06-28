#include <math.h>
#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx12.h"
#include "kiero.h"
#include "uglhook.h"

#include "gui.h"

#define clamp(x, a, b) ((x) < (a) ? (a) : (x) > (b) ? (b) : (x))
#define SBV(v, b) ((v) |= (b))
#define CBV(v, b) ((v) &= ~(b))
#define BTV(v, b) ((v) & (b))

static const char *MODES[] = { "FirstPerson", "Front", "Placed" };

GUI_t gGui = {0};

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(
  HWND hWnd,
  UINT msg,
  WPARAM wParam,
  LPARAM lParam);

static void gui_wndProcEx(
  HWND hWnd,
  UINT uMsg,
  WPARAM wParam,
  LPARAM lParam,
  UINT *uBlock,
  void *lpUser
) {
  ImGuiIO &io = ImGui::GetIO();

  ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam);

  switch (uMsg) {
    case WM_LBUTTONDOWN:
    case WM_LBUTTONDBLCLK:
    case WM_LBUTTONUP:
      *uBlock = io.WantCaptureMouse;
      return;
    case WM_KEYDOWN:
      if (gGui.state.overrideMode == 1 && gGui.state.enable) {
        if (
          wParam == 'W'
          || wParam == 'A'
          || wParam == 'S'
          || wParam == 'D'
          || wParam == VK_SPACE
          || wParam == VK_SHIFT
        )
          *uBlock = 1;
        else 
          *uBlock = io.WantCaptureKeyboard;
      } else
        *uBlock = io.WantCaptureKeyboard;
      return;
    case WM_CHAR:
      *uBlock = 1;
      return;
  }

  *uBlock = 0;
}

i08 gui_waitForDll() {
  i32 ctr = 0;
  while (!GetModuleHandleA("dxgi.dll") || !GetModuleHandleA("d3d12.dll")) {
    Sleep(100);
    ctr++;
    // Wait for 30 seconds.
    if (ctr > 300000)
      return 0;
  }
  return 1;
}

i08 gui_init() {
  // We assume that the gGui is safely deinitialized.
  memset(&gGui, 0, sizeof(GUI_t));
  QueryPerformanceFrequency((LARGE_INTEGER *)&gGui.performFreq);
  gGui.hInit = CreateEventW(NULL, 0, 0, NULL);

  return D3D12Hooks::init(
    [](const DXGI_SWAP_CHAIN_DESC *sDesc, void *lpUser) -> void {
      SetEvent(gGui.hInit);
      
      f32 dpiScale;
      ImGui::CreateContext();
      ImGui::StyleColorsDark();

      ImGuiIO &io = ImGui::GetIO(); (void)io;
      io.Fonts->AddFontDefault();
      io.IniFilename = NULL;

      ImGuiStyle& style = ImGui::GetStyle();
      dpiScale = ImGui_ImplWin32_GetDpiScaleForHwnd(sDesc->OutputWindow);
      style.ScaleAllSizes(dpiScale);
      io.FontGlobalScale = dpiScale;

      ImGui_ImplWin32_Init(UniHookGlobals::mainWindow);
      ImGui_ImplDX12_Init(
        D3D12Hooks::gDevice,
        D3D12Hooks::gBufferCount,
        sDesc->BufferDesc.Format,
        D3D12Hooks::gHeapSRV,
        D3D12Hooks::gHeapSRV->GetCPUDescriptorHandleForHeapStart(),
        D3D12Hooks::gHeapSRV->GetGPUDescriptorHandleForHeapStart()
      );
      ImGui_ImplDX12_CreateDeviceObjects();

      InputHandler::init(
        UniHookGlobals::mainWindow,
        gui_wndProcEx,
        nullptr
      );

      gGui.hWnd = UniHookGlobals::mainWindow;
    },
    [](void *lpUser) -> void {
      (void)gui_update();
    },
    [](void *lpUser) -> void {
      InputHandler::deinit(UniHookGlobals::mainWindow);
      ImGui_ImplDX12_Shutdown();
      ImGui_ImplWin32_Shutdown();
      ImGui::DestroyContext();
    },
    nullptr
  );

  return 1;
}

i08 gui_deinit() {
  D3D12Hooks::deinit();
  if (gGui.hInit)
    CloseHandle(gGui.hInit);
  return 1;
}

i08 gui_waitForInit() {
  i08 r = 1;
  if (WaitForSingleObject(gGui.hInit, 30000) != WAIT_OBJECT_0)
    r = 0;
  CloseHandle(gGui.hInit);
  gGui.hInit = NULL;
  return r;
}

static inline void gui_displayTips(i08 sameLine, const char *desc) {
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

static inline void gui_subMenuSet() {
  ImGui::SeparatorText("Set Camera Params");

  // Camera position input.
  ImGui::Checkbox("##cb1", (bool *)&gGui.state.overridePos);
  ImGui::SameLine();
  ImGui::InputFloat3("Pos", (float *)&gGui.state.pos);

  // Camera facing input.
  ImGui::Checkbox("##cb2", (bool *)&gGui.state.overrideDir);
  ImGui::SameLine();
  ImGui::InputFloat2("Facing", (float *)&gGui.state.rot);

  // Clamp camera facing.
  gGui.state.rot.x = fmodf(gGui.state.rot.x, 360.0f);
  gGui.state.rot.y = clamp(gGui.state.rot.y, -89.5f, 89.5f);

  // Camera scale input.
  ImGui::Checkbox("##cb3", (bool *)&gGui.state.overrideScale);
  ImGui::SameLine();
  ImGui::DragFloat("Scale", &gGui.state.scale, .01f, 0.0f, 1.0f);

  // Camera focus(blur) input.
  ImGui::Checkbox("##cb4", (bool *)&gGui.state.overrideFocus);
  ImGui::SameLine();
  ImGui::DragFloat("Focus", &gGui.state.focus, .01f, 0.0f, 1.0f);

  // Camera brightness input.
  ImGui::Checkbox("##cb5", (bool *)&gGui.state.overrideBrightness);
  ImGui::SameLine();
  ImGui::DragFloat("Brightness", &gGui.state.brightness, .01f, 0.0f, 1.0f);
}

static inline void gui_subMenuFreecam() {
  ImGui::SeparatorText("Free camera");
  ImGui::Checkbox("Axial", (bool *)&gGui.state.freecamAxial);
  gui_displayTips(
    true,
    "The camera won't move based on orientation when enabled this.");
  ImGui::Checkbox("Check collision", (bool *)&gGui.state.freecamCollision);
  ImGui::DragFloat("Speed", &gGui.state.freecamSpeed, .01f, 0, 100.0f);
  if (ImGui::Button("Reset pos"))
    gGui.state.resetPosFlag = 1;
}

static inline void gui_subMenuFPV() {
  ImGui::SeparatorText("FPV");
  if (ImGui::Button("Reset pos"))
    gGui.state.resetPosFlag = 1;
}

/**
 * Handle keyboard input for freecam mode.
 */
static void gui_keyboardFreecam() {
  v4f r = v4fnew(0.0f, 0.0f, 0.0f, 0.0f);

  if (ImGui::IsKeyDown(ImGuiKey_W))
    r.z += 1.0f;
  if (ImGui::IsKeyDown(ImGuiKey_A))
    r.x += 1.0f;
  if (ImGui::IsKeyDown(ImGuiKey_S))
    r.z -= 1.0f;
  if (ImGui::IsKeyDown(ImGuiKey_D))
    r.x -= 1.0f;
  if (ImGui::IsKeyDown(ImGuiKey_Space))
    r.y += 1.0f;
  if (ImGui::IsKeyDown(ImGuiKey_LeftShift))
    r.y -= 1.0f;

  gGui.state.movementInput = v4fnormalize(r);
}

/**
 * Render ui and handle keyboard inputs.
 */
i08 gui_update() {
  ImGuiIO &io = ImGui::GetIO();
  (void)io;

  if (GetAsyncKeyState(VK_INSERT) & 0x1)
    gGui.isOpen = !gGui.isOpen;

  ImGuiStyle &style = ImGui::GetStyle();
  style.ScrollbarRounding = 0.0f;
  style.WindowTitleAlign.x = 0.5f;
  style.WindowTitleAlign.y = 0.5f;

  ImGui_ImplDX12_NewFrame();
  ImGui_ImplWin32_NewFrame();
  ImGui::NewFrame();

  // Draw menus.
  if (gGui.isOpen) {
    // Title.
    ImGui::Begin(
      "hSC Menu",
      nullptr,
      ImGuiWindowFlags_None
    );

    // General options.
    if (ImGui::Checkbox("Take over", (bool *)&gGui.state.enable))
      gGui.state.resetPosFlag = 1;
    ImGui::Combo("Use mode", &gGui.state.cameraMode, MODES, IM_ARRAYSIZE(MODES));
    ImGui::Checkbox("No UI", (bool *)&gGui.state.noOriginalUi);
    gui_displayTips(
      true,
      "Please adjust the parameters before select this item.");

    ImGui::RadioButton("Set", &gGui.state.overrideMode, 0);
    ImGui::SameLine();
    ImGui::RadioButton("Freecam", &gGui.state.overrideMode, 1);
    ImGui::SameLine();
    ImGui::RadioButton("FPV", &gGui.state.overrideMode, 2);

    if (gGui.state.overrideMode == 0)
      gui_subMenuSet();
    if (gGui.state.overrideMode == 1)
      gui_subMenuFreecam();
    if (gGui.state.overrideMode == 2)
      gui_subMenuFPV();

    // Overlay window FPS display.
    ImGui::Text("Overlay %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);

    ImGui::End();
  }

  // Handle keyboard input.
  if (gGui.state.overrideMode == 1)
    gui_keyboardFreecam();

  // Rendering.
  ImGui::EndFrame();

  D3D12Hooks::FrameContext& currentFrameContext
    = D3D12Hooks::gFrameContext[D3D12Hooks::gSavedSwapChain->GetCurrentBackBufferIndex()];

  currentFrameContext.commandAllocator->Reset();

  D3D12_RESOURCE_BARRIER barrier;
  barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
  barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
  barrier.Transition.pResource = currentFrameContext.mainRenderTargetResource;
  barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
  barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
  barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;

  D3D12Hooks::gCommandList->Reset(
    currentFrameContext.commandAllocator,
    nullptr);
  D3D12Hooks::gCommandList->ResourceBarrier(1, &barrier);
  D3D12Hooks::gCommandList->OMSetRenderTargets(
    1,
    &currentFrameContext.mainRenderTargetDescriptor,
    FALSE,
    nullptr);
  D3D12Hooks::gCommandList->SetDescriptorHeaps(1, &D3D12Hooks::gHeapSRV);

  ImGui::Render();
  ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), D3D12Hooks::gCommandList);

  barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
  barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;

  D3D12Hooks::gCommandList->ResourceBarrier(1, &barrier);
  D3D12Hooks::gCommandList->Close();

  D3D12Hooks::gCommandQueue->ExecuteCommandLists(
    1,
    reinterpret_cast<ID3D12CommandList* const*>(&D3D12Hooks::gCommandList));

  return 1;
}


