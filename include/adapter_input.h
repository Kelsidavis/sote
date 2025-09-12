#ifndef ADAPTER_INPUT_H
#define ADAPTER_INPUT_H

#include <SDL2/SDL.h>
#include <stdint.h>
#include <windows.h>

// PROV: Evidence sources from runtime.apis.json
// DirectInputCreateA: VA_0x401000+, IAT_0x4c4ce8
// PeekMessageA/GetMessageA: VA_0x401000+, IAT_0x4c4bf0/0x4c4bfc
// DispatchMessageA/TranslateMessage: VA_0x401000+, IAT_0x4c4bd8/0x4c4bf6

// Input context for SDL2 event handling
typedef struct {
    SDL_Window *window;
    uint8_t key_state[SDL_NUM_SCANCODES];
    uint32_t mouse_x, mouse_y;
    uint32_t mouse_buttons;
    uint32_t initialized;
} INPUT_CONTEXT;

// Windows message structure
typedef struct {
    HWND hwnd;
    UINT message;
    WPARAM wParam;
    LPARAM lParam;
    DWORD time;
    POINT pt;
} MSG_ADAPTER;

// Message processing
// PROV: PeekMessageA call sites at VA_0x401000+, IAT_0x4c4bf0
BOOL adapter_PeekMessageA(MSG_ADAPTER *lpMsg, HWND hWnd, UINT wMsgFilterMin, UINT wMsgFilterMax, UINT wRemoveMsg);

// PROV: GetMessageA call sites at VA_0x401000+, IAT_0x4c4bfc  
BOOL adapter_GetMessageA(MSG_ADAPTER *lpMsg, HWND hWnd, UINT wMsgFilterMin, UINT wMsgFilterMax);

// PROV: DispatchMessageA call sites at VA_0x401000+, IAT_0x4c4bd8
LRESULT adapter_DispatchMessageA(const MSG_ADAPTER *lpMsg);

// PROV: TranslateMessage call sites at VA_0x401000+, IAT_0x4c4bf6
BOOL adapter_TranslateMessage(const MSG_ADAPTER *lpMsg);

// DirectInput adapter functions
// PROV: DirectInputCreateA call sites at VA_0x401000+, IAT_0x4c4ce8
HRESULT adapter_DirectInputCreateA(HINSTANCE hinst, DWORD dwVersion, void **ppDI, IUnknown *punkOuter);

// Keyboard input simulation
// PROV: Expected keyboard input for game controls
typedef struct {
    uint8_t keys[256];
    uint32_t initialized;
} DINPUT_KEYBOARD;

// Mouse input simulation
typedef struct {
    int32_t x, y, z;
    uint8_t buttons[8];
    uint32_t initialized;
} DINPUT_MOUSE;

// DirectInput device creation
HRESULT adapter_CreateDevice(void *pDI, const GUID *rguid, void **lplpDirectInputDevice, IUnknown *pUnkOuter);

// Device operations
HRESULT adapter_SetDataFormat(void *device, const void *lpdf);
HRESULT adapter_SetCooperativeLevel(void *device, HWND hwnd, DWORD dwFlags);
HRESULT adapter_Acquire(void *device);
HRESULT adapter_Unacquire(void *device);
HRESULT adapter_GetDeviceState(void *device, DWORD cbData, LPVOID lpvData);

// Utility functions
int adapter_input_init(SDL_Window *window);
void adapter_input_shutdown(void);
void adapter_input_update(void);
INPUT_CONTEXT* adapter_get_input_context(void);

// Convert SDL events to Windows messages
void adapter_sdl_to_windows_event(const SDL_Event *sdl_event, MSG_ADAPTER *win_msg);

// Windows message constants
#define WM_NULL             0x0000
#define WM_CREATE           0x0001
#define WM_DESTROY          0x0002
#define WM_MOVE             0x0003
#define WM_SIZE             0x0005
#define WM_ACTIVATE         0x0006
#define WM_SETFOCUS         0x0007
#define WM_KILLFOCUS        0x0008
#define WM_PAINT            0x000F
#define WM_CLOSE            0x0010
#define WM_QUIT             0x0012
#define WM_KEYDOWN          0x0100
#define WM_KEYUP            0x0101
#define WM_CHAR             0x0102
#define WM_SYSKEYDOWN       0x0104
#define WM_SYSKEYUP         0x0105
#define WM_SYSCHAR          0x0106
#define WM_MOUSEMOVE        0x0200
#define WM_LBUTTONDOWN      0x0201
#define WM_LBUTTONUP        0x0202
#define WM_RBUTTONDOWN      0x0204
#define WM_RBUTTONUP        0x0205
#define WM_MBUTTONDOWN      0x0207
#define WM_MBUTTONUP        0x0208

// PeekMessage flags
#define PM_NOREMOVE         0x0000
#define PM_REMOVE           0x0001
#define PM_NOYIELD          0x0002

// Virtual key codes (subset used by game)
#define VK_ESCAPE           0x1B
#define VK_SPACE            0x20
#define VK_LEFT             0x25
#define VK_UP               0x26
#define VK_RIGHT            0x27
#define VK_DOWN             0x28
#define VK_RETURN           0x0D
#define VK_TAB              0x09
#define VK_SHIFT            0x10
#define VK_CONTROL          0x11
#define VK_MENU             0x12

// DirectInput error codes
#define DI_OK                       0x00000000
#define DIERR_GENERIC               0x80004005
#define DIERR_INVALIDPARAM          0x80070057
#define DIERR_OUTOFMEMORY           0x8007000E
#define DIERR_NOTINITIALIZED        0x80070015
#define DIERR_ALREADYINITIALIZED    0x800704DF

#endif // ADAPTER_INPUT_H