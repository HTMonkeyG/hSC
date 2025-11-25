// ----------------------------------------------------------------------------
// API exports of HT's Mod Loader.
// Copyright (c) HTMonkeyG 2025
// https://www.github.com/HTMonkeyG/HTML-Sky
// ----------------------------------------------------------------------------

// #pragma once
#ifndef __HTMODLOADER_H__
#define __HTMODLOADER_H__

// Throws an error when compiled on other architectures.
#if !(defined(_M_X64) || defined(_WIN64) || defined(__x86_64__) || defined(__amd64__))
#error HT's Mod Loader and it's related mods is only avaliable on x86-64!
#endif

// Mod loader version.
// Version number is used for pre-processing statements handling version
// compatibility.
#define HTML_VERSION 10801
#define HTML_VERSION_NAME "1.8.1"

#define HTMLAPI __stdcall
#ifndef HTMLAPIATTR
#define HTMLAPIATTR
#endif

// Includes.
#include <windows.h>
#include <stdarg.h>

#ifdef __cplusplus
extern "C" {
#endif

// ----------------------------------------------------------------------------
// [SECTION] HTML basic APIs and type declarations.
// ----------------------------------------------------------------------------

// Whether the execution was successful or not.
typedef enum {
  HT_FAIL = 0,
  HT_SUCCESS = 1
} HTStatus;

// Game editions. Defined by backends.
typedef int HTGameEdition;

// Uninitialized state.
#define HT_ImplNull_EditionUnknown 0
// Only for debug use, don't check edition compatibility.
#define HT_ImplNull_EditionAll 0xFFFFFFFF

// Game status.
typedef struct {
  // Base address of game executable file.
  LPVOID baseAddr;
  // The edition of the game.
  HTGameEdition edition;
  // The window handle of the game.
  HWND window;
  // The process id of the game.
  DWORD pid;
} HTGameStatus;

// Function prototype.
typedef LPVOID (HTMLAPI *PFN_HTVoidFunction)(
  void);

// Handle.
typedef LPVOID HTHandle;

/* Mod exported function prototypes. */

// Gui renderer.
typedef void (HTMLAPI *PFN_HTModRenderGui)(
  float, LPVOID);
// Initialize event
typedef HTStatus (HTMLAPI *PFN_HTModOnInit)(
  LPVOID);
// Mod enable event
typedef HTStatus (HTMLAPI *PFN_HTModOnEnable)(
  LPVOID);

/**
 * Get the loader version which has loaded the mod.
 */
HTMLAPIATTR void HTMLAPI HTGetLoaderVersion(
  UINT32 *result);

/**
 * Get the loader version name which has loaded the mod.
 */
HTMLAPIATTR void HTMLAPI HTGetLoaderVersionName(
  LPSTR result,
  UINT32 max);

/**
 * Get game status object.
 */
HTMLAPIATTR void HTMLAPI HTGetGameStatus(
  HTGameStatus *status);

/**
 * Get the folder where the game executable file is located.
 */
HTMLAPIATTR void HTMLAPI HTGetGameExeFolder(
  LPSTR result,
  UINT64 maxLen);

/**
 * Get the folder where the mods is located. In most cases, the same as add
 * "\\htmods" to HTGetGameExeFolder()'s result.
 */
HTMLAPIATTR void HTMLAPI HTGetModFolder(
  LPSTR result,
  UINT64 maxLen);

/**
 * Get module handle from name. If module is nullptr, then returns the handle
 * of the caller.
 */
HTMLAPIATTR HMODULE HTMLAPI HTGetModuleHandle(
  LPCSTR module);

typedef enum {
  HTModInfoFields_ModName = 1,
  HTModInfoFields_PackageName,
  HTModInfoFields_Folder
} HTModInfoFields_;
typedef UINT32 HTModInfoFields;

/**
 * Expand mod info from manifest.
 * 
 * Returns copied the number of bytes written to the buffer pointed to by
 * `out`.
 * 
 * If the function succeeds and `maxLen` is 0, the return value is the required
 * size, in bytes, for the buffer indicated by `out`.
 */
HTMLAPIATTR UINT32 HTMLAPI HTGetModInfoFrom(
  HTHandle hManifest,
  HTModInfoFields info,
  LPVOID out,
  UINT32 maxLen);

/**
 * Get the name of currently active backends.
 * 
 * The name of the backends MUST NOT longer than 32 bytes, including '\0'.
 */
HTMLAPIATTR void HTMLAPI HTGetActiveBackendName(
  LPSTR gl,
  LPSTR game);

// Error codes. Partially the same as winerror.h
typedef int HTError;
typedef enum {
  HTError_Success = 0,
  // ERROR_ACCESS_DENIED.
  HTError_AccessDenied = 5,
  // ERROR_INVALID_HANDLE.
  HTError_InvalidHandle = 6,
  // ERROR_INVALID_PARAMETER.
  HTError_InvalidParam = 87,
  // ERROR_INSUFFICIENT_BUFFER.
  HTError_InsufficientBuffer = 122,
  // ERROR_MOD_NOT_FOUND.
  HTError_ModuleNotFound = 126,
  // ERROR_ALREADY_EXISTS.
  HTError_AlreadyExists = 183,
  // ERROR_NO_MORE_ITEMS.
  HTError_NoMoreItems = 259,
  // ERROR_NO_MORE_MATCHES.
  HTError_NoMoreMatches = 626,
  // ERROR_NOT_FOUND.
  HTError_NotFound = 1168
} HTError_;

// Set the last error code of HTML API.
HTMLAPIATTR void HTMLAPI HTSetLastError(
  HTError dwError);

// Get the last error code of HTML API.
HTMLAPIATTR HTError HTMLAPI HTGetLastError();

// For calling ImGui from DLLs that can't link ImGui from htmodloader.lib.
typedef struct {
  LPVOID pImGui;
  PFN_HTVoidFunction pImAllocatorAllocFunc;
  PFN_HTVoidFunction pImAllocatorFreeFunc;
  LPVOID pImAllocatorUserData;
} HTImGuiContexts;

/**
 * Get ImGui global context for those mods which can't link ImGui from
 * htmodloader.lib.
 * 
 * Call this function inside HTModRenderGui(), on the first frame, and set the
 * context of the mod's own ImGui library after calling ImGui functions.
 * 
 * Note that all ImGui drawing calls, whatever with or without the loader,
 * should only be put inside HTModRenderGui().
 */
HTMLAPIATTR HTStatus HTMLAPI HTImGuiDispatch(
  HTImGuiContexts *context);

typedef enum {
  HTOptionType_Invalid = 0,
  HTOptionType_Bool,
  HTOptionType_Double,
  HTOptionType_String
} HTOptionType;

/**
 * Get customized option value with the specified key.
 * 
 * For a string value, if `cch` != NULL and `data` == NULL, then the function
 * only returns the required byte count in `cch`. If `data` != NULL, the function
 * writes the string and returns the written byte count in `cch`, the function
 * won't exceed the buffer size originally specified by `cch`.
 * 
 * For other values, `cch` is ignored.
 */
HTMLAPIATTR HTStatus HTMLAPI HTOptionGetCustom(
  HMODULE hModule,
  LPCSTR key,
  HTOptionType type,
  LPVOID data,
  UINT32 *cch);

/**
 * Set customized option value with the specified key.
 * 
 * Options will be stored in options.json. DO NOT call this function frequently
 * or use it to store large volumes of data.
 */
HTMLAPIATTR HTStatus HTMLAPI HTOptionSetCustom(
  HMODULE hModule,
  LPCSTR key,
  HTOptionType type,
  LPCVOID data);

// ----------------------------------------------------------------------------
// [SECTION] HTML assembly patch APIs.
// ----------------------------------------------------------------------------

// Enable or disable all hooks or patches created by the specified mod.
#define HT_ALL_HOOKS NULL

// Method for obtaining the final address.
typedef enum {
  // The Signature represents the function body.
  HT_SCAN_DIRECT = 0,
  // The signature represents the E8 or E9 instruction that calls the function.
  HT_SCAN_E8,
  // The signature represents the FF15 instruction that calls the function.
  HT_SCAN_FF15,
} HTSigScanType;

// Signature code config.
typedef struct {
  // Signature code.
  LPCSTR sig;
  // Method for obtaining the final address.
  HTSigScanType indirect;
  // The byte offset of 0xE8 or 0x15 byte for HT_SCAN_E8 and HT_SCAN_FF15, or
  // the byte offset to the first instruction for HT_SCAN_DIRECT.
  UINT32 offset;
} HTAsmSig;

// Function address config.
typedef struct {
  // Function name, only for debug use.
  LPCSTR name;
  // The address of the scanned function.
  LPVOID fn;
  // The address of the detour function if hooked.
  LPVOID detour;
  // The address of the trampoline function if hooked.
  LPVOID origin;
} HTAsmFunction;

/**
 * Scan with signature. Note that HTSigScan functions won't call HTSetLastError
 * to set the error code.
 */
HTMLAPIATTR LPVOID HTMLAPI HTSigScan(
  const HTAsmSig *signature);

/**
 * Scan a single function.
 */
HTMLAPIATTR LPVOID HTMLAPI HTSigScanFunc(
  const HTAsmSig *signature,
  HTAsmFunction *func);

/**
 * Scan an array of functions.
 */
HTMLAPIATTR HTStatus HTMLAPI HTSigScanFuncEx(
  const HTAsmSig **signature,
  HTAsmFunction **func,
  UINT32 count);

/**
 * Create hook with MinHook. This function won't record the function name.
 */
HTMLAPIATTR HTStatus HTMLAPI HTAsmHookCreateRaw(
  HMODULE hModuleOwner,
  LPVOID fn,
  LPVOID detour,
  LPVOID *origin);

/**
 * Creates a hook for the specified API function with MinHook.
 */
HTMLAPIATTR HTStatus HTMLAPI HTAsmHookCreateAPI(
  HMODULE hModuleOwner,
  LPCWSTR hModule,
  LPCSTR func,
  LPVOID detour,
  LPVOID *origin,
  LPVOID *target);

/**
 * Create a hook from HTHookFunction struct. We must bind the hooks to the mod,
 * so when a mod is dynamically unloaded, we can destroy all its hooks.
 */
HTMLAPIATTR HTStatus HTMLAPI HTAsmHookCreate(
  HMODULE hModuleOwner,
  HTAsmFunction *func);

/**
 * Enable hook on specified function.
 */
HTMLAPIATTR HTStatus HTMLAPI HTAsmHookEnable(
  HMODULE hModuleOwner,
  LPVOID fn);

/**
 * Disable hook on specified function.
 */
HTMLAPIATTR HTStatus HTMLAPI HTAsmHookDisable(
  HMODULE hModuleOwner,
  LPVOID fn);

/**
 * [Future] Create a patch on specified address.
 */
HTMLAPIATTR HTStatus HTMLAPI HTAsmPatchCreate(
  HMODULE hModuleOwner,
  LPVOID target,
  LPCVOID data,
  UINT64 size);

/**
 * [Future] Enable patch.
 */
HTMLAPIATTR HTStatus HTMLAPI HTAsmPatchEnable(
  HMODULE hModuleOwner,
  LPVOID target);

/**
 * [Future] Disable patch.
 */
HTMLAPIATTR HTStatus HTMLAPI HTAsmPatchDisable(
  HMODULE hModuleOwner,
  LPVOID target);

// ----------------------------------------------------------------------------
// [SECTION] HTML memory manager APIs.
// ----------------------------------------------------------------------------

/**
 * Allocate a sized memory block.
 */
HTMLAPIATTR LPVOID HTMLAPI HTMemAlloc(
  UINT64 size);

/**
 * Allocate space for an array of `count` objects, each of `size` bytes.
 * Different from calloc(), HTMemNew won't initialize the memory block.
 */
HTMLAPIATTR LPVOID HTMLAPI HTMemNew(
  UINT64 count,
  UINT64 size);

/**
 * Free a memory block allocated with HTMemAlloc() or HTMemNew(). Returns
 * HT_FAIL when the pointer is invalid or is already freed.
 * 
 * Mod needs to reset pointer variables to prevent dangling pointers.
 */
HTMLAPIATTR HTStatus HTMLAPI HTMemFree(
  LPVOID pointer);

// ----------------------------------------------------------------------------
// [SECTION] HTML mod communication APIs.
// ----------------------------------------------------------------------------

// Event callback.
typedef void (HTMLAPI *PFN_HTEventCallback)(
  const LPVOID data);

#define HT_INVALID_HANDLE NULL

/**
 * Get the address of a registered function.
 */
HTMLAPIATTR PFN_HTVoidFunction HTMLAPI HTGetProcAddr(
  HMODULE hModule,
  LPCSTR name);

/**
 * Get a handle for the mod manifest.
 */
HTMLAPIATTR HTHandle HTMLAPI HTGetModManifest(
  HMODULE hModule);

/**
 * Register a function with name. Registered function can be called by other
 * mods with HTGetProcAddr(). If the same name is passed in and called more
 * than once, the value of the last call will be saved.
 * 
 * It is recommended to use namespace strings like `namespace:foobar` when
 * registering functions.
 */
HTMLAPIATTR HTStatus HTMLAPI HTCommRegFunction(
  HMODULE hModule,
  LPCSTR name,
  PFN_HTVoidFunction func);

/**
 * Register an event listener with given event name.
 * 
 * The callback function should not modify the content pointed to by the
 * `data` pointer. The callback function must assume that the data pointer
 * is only valid before the callback function returns.
 */
HTMLAPIATTR HTStatus HTMLAPI HTCommOnEvent(
  LPCSTR name,
  PFN_HTEventCallback callback);

#define HTCommAddEventListener HTCommOnEvent

/**
 * Remove a registered event listener.
 */
HTMLAPIATTR HTStatus HTMLAPI HTCommOffEvent(
  LPCSTR name,
  PFN_HTEventCallback callback);

#define HTCommRemoveEventListener HTCommOffEvent
  
/**
 * Trigger an event with specified data.
 * 
 * DO NOT emit the event itself in the callback function.
 */
HTMLAPIATTR HTStatus HTMLAPI HTCommEmitEvent(
  LPCSTR name,
  LPVOID reserved,
  LPVOID data);

// ----------------------------------------------------------------------------
// [SECTION] HTML hotkey register APIs.
// ----------------------------------------------------------------------------

// Modified from ImGui to keep compatibility.
// NOTE: HTKeyCodes is not completely compatible with ImGuiKey, specially
// in mouse inputs. Use HTKeyToImGuiKey() to convert to ImGuiKey.
typedef enum {
  HTKey_None = 0,

  HTKey_NamedKey_BEGIN = 512,
  HTKey_Tab = 512,
  HTKey_LeftArrow,
  HTKey_RightArrow,
  HTKey_UpArrow,
  HTKey_DownArrow,
  HTKey_PageUp,
  HTKey_PageDown,
  HTKey_Home,
  HTKey_End,
  HTKey_Insert,
  HTKey_Delete,
  HTKey_Backspace,
  HTKey_Space,
  HTKey_Enter,
  HTKey_Escape,
  HTKey_LeftCtrl, HTKey_LeftShift, HTKey_LeftAlt, HTKey_LeftSuper,
  HTKey_RightCtrl, HTKey_RightShift, HTKey_RightAlt, HTKey_RightSuper,
  HTKey_Menu,
  HTKey_0, HTKey_1, HTKey_2, HTKey_3, HTKey_4, HTKey_5, HTKey_6, HTKey_7, HTKey_8, HTKey_9,
  HTKey_A, HTKey_B, HTKey_C, HTKey_D, HTKey_E, HTKey_F, HTKey_G, HTKey_H, HTKey_I, HTKey_J,
  HTKey_K, HTKey_L, HTKey_M, HTKey_N, HTKey_O, HTKey_P, HTKey_Q, HTKey_R, HTKey_S, HTKey_T,
  HTKey_U, HTKey_V, HTKey_W, HTKey_X, HTKey_Y, HTKey_Z,
  HTKey_F1, HTKey_F2, HTKey_F3, HTKey_F4, HTKey_F5, HTKey_F6,
  HTKey_F7, HTKey_F8, HTKey_F9, HTKey_F10, HTKey_F11, HTKey_F12,
  HTKey_F13, HTKey_F14, HTKey_F15, HTKey_F16, HTKey_F17, HTKey_F18,
  HTKey_F19, HTKey_F20, HTKey_F21, HTKey_F22, HTKey_F23, HTKey_F24,
  // '
  HTKey_Apostrophe,
  // ,
  HTKey_Comma,
  // -
  HTKey_Minus,
  // .
  HTKey_Period,
  // /
  HTKey_Slash,
  // ;
  HTKey_Semicolon,
  // =
  HTKey_Equal,
  // [
  HTKey_LeftBracket,
  // \ (this text inhibit multiline comment caused by backslash)
  HTKey_Backslash,
  // ]
  HTKey_RightBracket,
  // `
  HTKey_GraveAccent,
  HTKey_CapsLock,
  HTKey_ScrollLock,
  HTKey_NumLock,
  HTKey_PrintScreen,
  HTKey_Pause,
  HTKey_Keypad0, HTKey_Keypad1, HTKey_Keypad2, HTKey_Keypad3, HTKey_Keypad4,
  HTKey_Keypad5, HTKey_Keypad6, HTKey_Keypad7, HTKey_Keypad8, HTKey_Keypad9,
  HTKey_KeypadDecimal,
  HTKey_KeypadDivide,
  HTKey_KeypadMultiply,
  HTKey_KeypadSubtract,
  HTKey_KeypadAdd,
  HTKey_KeypadEnter,
  HTKey_KeypadEqual,
  // Available on some keyboard/mouses. Often referred as "Browser Back"
  HTKey_AppBack,
  HTKey_AppForward,
  // Non-US backslash.
  HTKey_Oem102,

  // Mouse inputs.
  HTKey_Mouse_BEGIN,
  HTKey_MouseLeft = HTKey_Mouse_BEGIN,
  HTKey_MouseRight,
  HTKey_MouseMiddle,
  HTKey_MouseX1,
  HTKey_MouseX2,
  // HTML external mouse wheel key codes. These key codes is different from
  // ImGuiKey_MouseWheelX or ImGuiKey_MouseWheelY, which uses analog inputs to
  // indicate the direction, the key codes below act as a single physical key
  // like those on keyboard.
  HTKey_MouseWheelUp,
  HTKey_MouseWheelDown,
  // Most users won't have horizontal mouse wheels. Why did I add these?
  HTKey_MouseWheelLeft,
  HTKey_MouseWheelRight,
  HTKey_Mouse_END,
  HTKey_NamedKey_END = HTKey_Mouse_END,

  HTKey_NamedKey_COUNT = HTKey_NamedKey_END - HTKey_NamedKey_BEGIN,

  HTKeyMod_None = 0,
  // Ctrl (non-macOS), Cmd (macOS)
  HTKeyMod_Ctrl = 1 << 12,
  // Shift
  HTKeyMod_Shift = 1 << 13,
  // Option/Menu
  HTKeyMod_Alt = 1 << 14,
  // Windows/Super (non-macOS), Ctrl (macOS)
  HTKeyMod_Super = 1 << 15,
} HTKeyCode;

// Key event properties.
typedef UINT32 HTKeyEventFlags;
typedef enum {
  HTKeyEventFlags_None = 0,
  HTKeyEventFlags_Down,
  HTKeyEventFlags_Up,
  HTKeyEventFlags_ChangeBind,
  HTKeyEventFlags_ResetBind,
  HTKeyEventFlags_MouseWheelDown,
  HTKeyEventFlags_MouseWheelUp,
  HTKeyEventFlags_MouseWheelLeft,
  HTKeyEventFlags_MouseWheelRight,

  // [Internal] Only for internal HTiHotkeyDispatch() function. The flags below
  // will never be set on callbacks.
  HTKeyEventFlags_Repeat = 1 << 16,
  HTKeyEventFlags_Blocked = 1 << 17,
  HTKeyEventFlags_Mask = 0xFFFF
} HTKeyEventFlags_;

// Key binding flags.
typedef UINT32 HTHotkeyFlags;
typedef enum {
  // Default value. The KeyDown events will be blocked when any ImGui window is
  // focused, due to io.WantCaptureKeyboard and io.WantCaptureMouse flags. Set
  // this flag when you want the key bind is only avaliable "in game".
  HTHotkeyFlags_None = 0,
  // If this flag is set, then the KeyDown events won't be blocked. For those
  // key binds need to preview in game.
  HTHotkeyFlags_NoBlock = 1 << 0,
  // Reserved.
  HTHotkeyFlags_BlockKeyUp = 1 << 1
} HTHotkeyFlags_;

// Determine how to intercept the key message.
typedef UINT32 HTKeyEventPreventFlags;
typedef enum {
  // Pass the event as normal.
  HTKeyEventPreventFlags_None = 0,
  // Prevent the game from receiving the key message. Setting this flag in any
  // of the callbacks will prevent events from being passed down.
  HTKeyEventPreventFlags_Game = 1 << 0,
  // Prevent the next event callback listening the key from receiving the key
  // message. We do not ensure the order of the callbacks, so this flag may
  // affect other mod's behaviour uncontrollable.
  HTKeyEventPreventFlags_Next = 1 << 1,
} HTKeyEventPreventFlags_;

// Key event data.
typedef struct {
  // [In] Handle of the key bind.
  HTHandle hKey;
  // [In] Key code of this event. For HTKeyEventFlags_Down and HTKeyEventFlags_Up,
  // this field is the key pressed. For HTKeyEventFlags_ChangeBind and 
  // HTKeyEventFlags_ResetBind, is the previous bound key.
  HTKeyCode key;
  // [In] Is the event a key press event. This field has been deprecated, reserved
  // for compatibility.
  unsigned char down;
  // [In] Key event flags, marked the type of this event.
  HTKeyEventFlags flags;

  // [Out] Determine how to intercept the key message.
  HTKeyEventPreventFlags preventFlags;
} HTKeyEvent;

// Hotkey callback.
typedef void (HTMLAPI *PFN_HTHotkeyCallback)(
  HTKeyEvent *);

/**
 * Get the name string of a key code.
 */
HTMLAPIATTR LPCSTR HTMLAPI HTHotkeyGetName(
  HTKeyCode key);

/**
 * Shortcut for passing HTHotkeyFlags_None to HTHotkeyRegisterEx()
 */
HTMLAPIATTR HTHandle HTMLAPI HTHotkeyRegister(
  HMODULE hModule,
  LPCSTR name,
  HTKeyCode defaultCode);

/**
 * Register a single key bind.
 */
HTMLAPIATTR HTHandle HTMLAPI HTHotkeyRegisterEx(
  HMODULE hModule,
  LPCSTR name,
  HTKeyCode defaultCode,
  HTHotkeyFlags flags);

/**
 * Get the bound key from a key binding. Returns HTKey_NamedKey_END if failed.
 */
HTMLAPIATTR HTKeyCode HTMLAPI HTHotkeyBindGet(
  HTHandle hKey);

/**
 * Change the bound key for a hotkey. If `key` is HTKey_None, then this
 * function resets the key bind.
 */
HTMLAPIATTR HTStatus HTMLAPI HTHotkeyBind(
  HTHandle hKey,
  HTKeyCode keyCode);

/**
 * Reset the key bind to default.
 */
HTMLAPIATTR HTStatus HTMLAPI HTHotkeyBindReset(
  HTHandle hKey);

/**
 * Check if a registered key has been pressed.
 * 
 * For a better performance, this function DOES NOT check the handle. Mods MUST
 * ensure the handle is valid.
 */
HTMLAPIATTR UINT32 HTMLAPI HTHotkeyPressed(
  HTHandle hKey);

/**
 * Register a callback for monitoring key state switching.
 * 
 * If this function is called multiple times for the same handle, the last
 * callback passed in will be recorded.
 */
HTMLAPIATTR HTStatus HTMLAPI HTHotkeyListen(
  HTHandle hKey,
  PFN_HTHotkeyCallback callback);

/**
 * Unregister the callback function for a hotkey.
 * 
 * For a better compatibility, `reserved` must be NULL.
 */
HTMLAPIATTR HTStatus HTMLAPI HTHotkeyUnlisten(
  HTHandle hKey,
  LPVOID reserved);

// ----------------------------------------------------------------------------
// [SECTION] HTML console text APIs.
// ----------------------------------------------------------------------------

/**
 * Prints text on the in-game console.
 * 
 * Use '§' to represent color code, §0~§f represents the color in the terminal
 * escape sequence, §#<DWORD> which DWORD is a hex color code in AABBGGRR
 * format represents full RGBA color. Color codes won't be displayed on the
 * console.
 */
HTMLAPIATTR HTStatus HTMLAPI HTTellText(
  LPCSTR format,
  ...);

HTMLAPIATTR HTStatus HTMLAPI HTTellTextV(
  LPCSTR format,
  va_list v);

/**
 * Prints raw text on the in-game console.
 * 
 * This function disables color escape sequences.
 */
HTMLAPIATTR HTStatus HTMLAPI HTTellRaw(
  LPCSTR format,
  ...);

HTMLAPIATTR HTStatus HTMLAPI HTTellRawV(
  LPCSTR format,
  va_list v);

// ----------------------------------------------------------------------------
// [SECTION] HTML persistent data storage APIs.
// ----------------------------------------------------------------------------

/**
 * Store the given data into LevelDB with the given key.
 * 
 * Both the key and value is considered as a binary buffer.
 */
HTMLAPIATTR HTStatus HTMLAPI HTDataStore(
  HMODULE hModule,
  const char *key,
  UINT64 keyLen,
  const char *value,
  UINT64 valueLen);

/**
 * Get a stored value with given key.
 */
HTMLAPIATTR char *HTMLAPI HTDataGet(
  HMODULE hModule,
  const char *key,
  UINT64 keyLen,
  UINT64 *valueLen);

/**
 * The key's length is calculated automatically by HTDataStoreStringKey()
 * and HTDataGetStringKey().
 * 
 * The key's length don't include the zero terminator.
 */
HTMLAPIATTR HTStatus HTMLAPI HTDataStoreStringKey(
  HMODULE hModule,
  const char *key,
  const char *value,
  UINT64 valueLen);

HTMLAPIATTR char *HTMLAPI HTDataGetStringKey(
  HMODULE hModule,
  const char *key,
  UINT64 *valueLen);

/**
 * Free the pointer returned by HTDataGet().
 */
HTMLAPIATTR void HTMLAPI HTDataFree(
  char *value);

#ifdef __cplusplus
}
#endif

#endif
