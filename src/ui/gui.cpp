#include <math.h>
#include <windows.h>

#include "imgui.h"
#ifndef USE_HTML
#include "imgui_impl_win32.h"
#include "imgui_impl_dx12.h"
#include "uglhook.h"
#endif
#include "includes/htmod.h"

#include "log.h"
#include "ui/ui.h"

static char gIniPath[260];
static wchar_t gPrefPath[260];

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

#ifndef USE_HTML

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
      *uBlock = io.WantCaptureKeyboard;

      if (!gState.enable)
        return;

      if (gState.majorMode == OM_FREE) {
        // Freecam mode.
        if (
          wParam == 'W'
          || wParam == 'A'
          || wParam == 'S'
          || wParam == 'D'
          || wParam == 'Q'
          || wParam == 'E'
          || wParam == VK_SPACE
          || wParam == VK_SHIFT
        )
          *uBlock = 1;
      } else if (gState.majorMode == OM_FPV) {
        // FPV mode.
        if (
          wParam == 'W'
          || wParam == 'A'
          || wParam == 'S'
          || wParam == 'D'
          || wParam == VK_SPACE
        )
          *uBlock = 1;
      }
      return;
    case WM_CHAR:
      *uBlock = 1;
      return;
    default:
      *uBlock = 0;
  }
}

/**
 * Wait for the dll to be loaded.
 */
i08 gui_waitForDll(DWORD *lastError) {
  i32 ctr = 0;
  while (!GetModuleHandleA("dxgi.dll") || !GetModuleHandleA("d3d12.dll")) {
    Sleep(100);
    ctr++;
    // Wait for 30 seconds.
    if (ctr > 300) {
      *lastError = GetLastError();
      return 0;
    }
  }
  return 1;
}

/**
 * Load saved preference data.
 */
i08 gui_loadPreferences(HMODULE hModule) {
  setupPaths(hModule, gPrefPath, gIniPath);
  return 0;
}

/**
 * Save preference data to disk.
 */
i08 gui_savePreferences(HMODULE hModule) {
  //setupPaths(hModule, );
  return 0;
}

/**
 * Initialize the gui.
 */
i08 gui_init(HMODULE hModule) {
  // We assume that the gGui is safely deinitialized.
  memset(&gGui, 0, sizeof(GUI_t));
  memset(&gState, 0, sizeof(GUIState_t));
  QueryPerformanceFrequency((LARGE_INTEGER *)&gGui.performFreq);
  gGui.hInit = CreateEventW(nullptr, 0, 0, nullptr);
  gGui.isOpen = 1;

  // Preload saved preference data to memory.
  gui_loadPreferences(hModule);

  return D3D12Hooks::init(
    [](const DXGI_SWAP_CHAIN_DESC *sDesc, void *lpUser) -> void {
      SetEvent(gGui.hInit);

      ImGui::CreateContext();
      ImGui::StyleColorsDark();

      ImGuiIO &io = ImGui::GetIO(); (void)io;
      io.Fonts->AddFontDefault();
      LOGI("Try to load imgui.ini at: %s\n", gIniPath);
      io.IniFilename = gIniPath;

      ImGuiStyle &style = ImGui::GetStyle();
      f32 dpiScale = ImGui_ImplWin32_GetDpiScaleForHwnd(sDesc->OutputWindow);
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
}

/**
 * Uninitialze the dx12 hooks.
 */
i08 gui_deinit() {
  D3D12Hooks::deinit();
  if (gGui.hInit)
    CloseHandle(gGui.hInit);
  return 1;
}

/**
 * Wait for the first IDXGISwapChain::Present() call.
 */
i08 gui_waitForInit() {
  i08 r = 1;
  if (WaitForSingleObject(gGui.hInit, 30000) != WAIT_OBJECT_0)
    r = 0;
  CloseHandle(gGui.hInit);
  gGui.hInit = nullptr;
  return r;
}

/**
 * Render ui and handle keyboard inputs.
 */
i08 gui_update() {
  // Press "~" key to show or hide.
  if (GetAsyncKeyState(VK_OEM_3) & 0x1)
    gGui.isOpen = !gGui.isOpen;

  ImGuiStyle &style = ImGui::GetStyle();
  style.ScrollbarRounding = 0.0f;
  style.WindowTitleAlign.x = 0.5f;
  style.WindowTitleAlign.y = 0.5f;

  ImGui_ImplDX12_NewFrame();
  ImGui_ImplWin32_NewFrame();
  ImGui::NewFrame();

  // Draw menus.
  if (gGui.isOpen)
    gui_windowMain();
  else
    gGui.showSettings = 0;
  if (gGui.showSettings)
    gui_windowSettings();

  // Handle keyboard input.
  if (gState.enable)
    gui_handleInput();

  // Rendering.
  ImGui::EndFrame();

  D3D12Hooks::FrameContext &currentFrameContext
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
    reinterpret_cast<ID3D12CommandList *const *>(&D3D12Hooks::gCommandList));

  return 1;
}

#else

#endif
