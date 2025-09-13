#include "adapter_input.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// PROV: SDL2 include disabled for RE-AGENT REBUILD m100 - using forward declarations
#if 0 /* disabled for RE-AGENT REBUILD m100 */
#include <SDL2/SDL.h>
// Forward declarations for SDL input functions
extern int SDL_PollEvent(SDL_Event *event);
extern const uint8_t* SDL_GetKeyboardState(int *numkeys);
extern uint32_t SDL_GetMouseState(int *x, int *y);
extern uint32_t SDL_GetTicks(void);
extern void SDL_StartTextInput(void);
extern void SDL_StopTextInput(void);
extern void SDL_Delay(uint32_t ms);
#endif

// SDL stub functions for cross-compilation builds
#if (defined(WIN32_BUILD) && !defined(SOTE_FORCE_SDL)) || !defined(HAVE_ADAPTER_SDL)
static int SDL_PollEvent(SDL_Event *event) { return 0; }
static const uint8_t* SDL_GetKeyboardState(int *numkeys) { static uint8_t keys[512] = {0}; return keys; }
static uint32_t SDL_GetMouseState(int *x, int *y) { return 0; }
static uint32_t SDL_GetTicks(void) { return 0; }
static void SDL_StartTextInput(void) { }
static void SDL_StopTextInput(void) { }
static void SDL_Delay(uint32_t ms) { }
// PROV: Stub for keycode to scancode conversion
typedef int SDL_Scancode;
static SDL_Scancode SDL_GetScancodeFromKey(int keycode) { 
    // Simple stub mapping for common keys
    if (keycode == SDLK_RETURN) return 40; // Return key scancode estimate
    if (keycode == SDLK_ESCAPE) return SDL_SCANCODE_ESCAPE;
    return SDL_SCANCODE_UNKNOWN;
}
#endif

// PROV: DirectInput/Windows message API replacements based on runtime.apis.json
// Evidence: DirectInputCreateA at IAT_0x4c4ce8 for input device creation
// Evidence: PeekMessageA at IAT_0x4c4bf0 for non-blocking event polling
// Evidence: GetMessageA/DispatchMessageA for main event loop processing

static INPUT_CONTEXT g_input_context = {0};
static MSG_ADAPTER g_message_queue[256];
static uint32_t g_queue_head = 0, g_queue_tail = 0;

// PROV: VA_0x401000+ PeekMessageA call sites from evidence
// Non-blocking message retrieval for game loop
BOOL adapter_PeekMessageA(MSG_ADAPTER *lpMsg, HWND hWnd, UINT wMsgFilterMin, UINT wMsgFilterMax, UINT wRemoveMsg)
{
    if (!lpMsg) return FALSE;
    
    // Update input state and process SDL events
    adapter_input_update();
    
    // Check if we have queued messages
    if (g_queue_head == g_queue_tail) {
        return FALSE; // No messages available
    }
    
    // Copy message from queue
    *lpMsg = g_message_queue[g_queue_head];
    
    // Remove from queue if requested
    if (wRemoveMsg & PM_REMOVE) {
        g_queue_head = (g_queue_head + 1) % 256;
    }
    
    return TRUE;
}

// PROV: GetMessageA call sites at VA_0x401000+, IAT_0x4c4bfc
// Blocking message retrieval (waits for events)  
BOOL adapter_GetMessageA(MSG_ADAPTER *lpMsg, HWND hWnd, UINT wMsgFilterMin, UINT wMsgFilterMax)
{
    if (!lpMsg) return FALSE;
    
    // Block until we have a message
    while (g_queue_head == g_queue_tail) {
        adapter_input_update();
        SDL_Delay(1); // Small delay to prevent busy waiting
    }
    
    // Copy and remove message from queue
    *lpMsg = g_message_queue[g_queue_head];
    g_queue_head = (g_queue_head + 1) % 256;
    
    // Return FALSE for WM_QUIT, TRUE for all other messages
    return (lpMsg->message != WM_QUIT);
}

// PROV: DispatchMessageA call sites at VA_0x401000+, IAT_0x4c4bd8  
LRESULT adapter_DispatchMessageA(const MSG_ADAPTER *lpMsg)
{
    if (!lpMsg) return 0;
    
    // For SDL adapter, we just acknowledge the message was processed
    // In a full implementation, this would call the window procedure
    return 0;
}

// PROV: TranslateMessage call sites at VA_0x401000+, IAT_0x4c4bf6
BOOL adapter_TranslateMessage(const MSG_ADAPTER *lpMsg)
{
    if (!lpMsg) return FALSE;
    
    // Translate key events to character events if needed
    if (lpMsg->message == WM_KEYDOWN) {
        // Could generate WM_CHAR message here if needed
        return TRUE;
    }
    
    return FALSE;
}

// PROV: DirectInputCreateA call sites at VA_0x401000+, IAT_0x4c4ce8
HRESULT adapter_DirectInputCreateA(HINSTANCE hinst, DWORD dwVersion, void **ppDI, IUnknown *punkOuter)
{
    if (!ppDI) return DIERR_INVALIDPARAM;
    
    // Initialize input system if not already done
    if (!g_input_context.initialized) {
        return DIERR_NOTINITIALIZED;
    }
    
    // Return input context as DirectInput interface
    *ppDI = &g_input_context;
    
    return DI_OK;
}

// DirectInput device operations (stubs for SDL compatibility)
HRESULT adapter_CreateDevice(void *pDI, const GUID *rguid, void **lplpDirectInputDevice, IUnknown *pUnkOuter)
{
    if (!lplpDirectInputDevice) return DIERR_INVALIDPARAM;
    
    // Return appropriate device structure based on GUID
    // For now, return generic input context
    *lplpDirectInputDevice = &g_input_context;
    
    return DI_OK;
}

HRESULT adapter_SetDataFormat(void *device, const void *lpdf)
{
    // SDL handles data format internally
    return DI_OK;
}

HRESULT adapter_SetCooperativeLevel(void *device, HWND hwnd, DWORD dwFlags)
{
    // SDL input is automatically cooperative
    return DI_OK;
}

HRESULT adapter_Acquire(void *device)
{
    // SDL input is acquired by default when window has focus
    return DI_OK;
}

HRESULT adapter_Unacquire(void *device)
{
    // SDL input is automatically released when window loses focus
    return DI_OK;
}

HRESULT adapter_GetDeviceState(void *device, DWORD cbData, LPVOID lpvData)
{
    if (!lpvData) return DIERR_INVALIDPARAM;
    
    // Copy current input state
    if (cbData >= sizeof(g_input_context.key_state)) {
        memcpy(lpvData, g_input_context.key_state, sizeof(g_input_context.key_state));
        return DI_OK;
    }
    
    return DIERR_INVALIDPARAM;
}

// Convert SDL events to Windows messages and queue them
void adapter_sdl_to_windows_event(const SDL_Event *sdl_event, MSG_ADAPTER *win_msg)
{
    if (!sdl_event || !win_msg) return;
    
    memset(win_msg, 0, sizeof(MSG_ADAPTER));
    win_msg->hwnd = (HWND)g_input_context.window;
    win_msg->time = SDL_GetTicks();
    
    switch (sdl_event->type) {
        case SDL_QUIT:
            win_msg->message = WM_QUIT;
            break;
            
        case SDL_KEYDOWN:
            win_msg->message = WM_KEYDOWN;
            win_msg->wParam = sdl_event->key.keysym.sym; // SDL key to VK_* mapping needed
            break;
            
        case SDL_KEYUP:
            win_msg->message = WM_KEYUP;
            win_msg->wParam = sdl_event->key.keysym.sym;
            break;
            
        case SDL_MOUSEBUTTONDOWN:
            if (sdl_event->button.button == SDL_BUTTON_LEFT) {
                win_msg->message = WM_LBUTTONDOWN;
            } else if (sdl_event->button.button == SDL_BUTTON_RIGHT) {
                win_msg->message = WM_RBUTTONDOWN; 
            } else if (sdl_event->button.button == SDL_BUTTON_MIDDLE) {
                win_msg->message = WM_MBUTTONDOWN;
            }
            win_msg->lParam = MAKELONG(sdl_event->button.x, sdl_event->button.y);
            break;
            
        case SDL_MOUSEBUTTONUP:
            if (sdl_event->button.button == SDL_BUTTON_LEFT) {
                win_msg->message = WM_LBUTTONUP;
            } else if (sdl_event->button.button == SDL_BUTTON_RIGHT) {
                win_msg->message = WM_RBUTTONUP;
            } else if (sdl_event->button.button == SDL_BUTTON_MIDDLE) {
                win_msg->message = WM_MBUTTONUP;
            }
            win_msg->lParam = MAKELONG(sdl_event->button.x, sdl_event->button.y);
            break;
            
        case SDL_MOUSEMOTION:
            win_msg->message = WM_MOUSEMOVE;
            win_msg->lParam = MAKELONG(sdl_event->motion.x, sdl_event->motion.y);
            break;
            
        case SDL_WINDOWEVENT:
            switch (sdl_event->window.event) {
                case SDL_WINDOWEVENT_CLOSE:
                    win_msg->message = WM_CLOSE;
                    break;
                case SDL_WINDOWEVENT_SIZE_CHANGED:
                    win_msg->message = WM_SIZE;
                    win_msg->lParam = MAKELONG(sdl_event->window.data1, sdl_event->window.data2);
                    break;
                case SDL_WINDOWEVENT_MOVED:
                    win_msg->message = WM_MOVE;
                    win_msg->lParam = MAKELONG(sdl_event->window.data1, sdl_event->window.data2);
                    break;
                default:
                    win_msg->message = WM_NULL;
                    break;
            }
            break;
            
        default:
            win_msg->message = WM_NULL;
            break;
    }
}

// Queue a message for later retrieval
static void queue_message(const MSG_ADAPTER *msg)
{
    if (!msg) return;
    
    uint32_t next_tail = (g_queue_tail + 1) % 256;
    if (next_tail != g_queue_head) { // Queue not full
        g_message_queue[g_queue_tail] = *msg;
        g_queue_tail = next_tail;
    }
}

// Main input update function - processes SDL events and updates state
void adapter_input_update(void)
{
    SDL_Event event;
    MSG_ADAPTER win_msg;
    
    // Process all pending SDL events
    while (SDL_PollEvent(&event)) {
        // Update input state
        if (event.type == SDL_KEYDOWN || event.type == SDL_KEYUP) {
            const uint8_t *state = SDL_GetKeyboardState(NULL);
            memcpy(g_input_context.key_state, state, SDL_NUM_SCANCODES);
        }
        
        if (event.type == SDL_MOUSEMOTION) {
            g_input_context.mouse_x = event.motion.x;
            g_input_context.mouse_y = event.motion.y;
        }
        
        if (event.type == SDL_MOUSEBUTTONDOWN || event.type == SDL_MOUSEBUTTONUP) {
            uint32_t button_mask = SDL_GetMouseState(NULL, NULL);
            g_input_context.mouse_buttons = button_mask;
        }
        
        // Convert to Windows message and queue
        adapter_sdl_to_windows_event(&event, &win_msg);
        if (win_msg.message != WM_NULL) {
            queue_message(&win_msg);
        }
    }
}

// Context management
int adapter_input_init(SDL_Window *window)
{
    if (g_input_context.initialized) return 0;
    
    g_input_context.window = window;
    
    // Enable text input for character events
    SDL_StartTextInput();
    
    // Clear input state
    memset(g_input_context.key_state, 0, sizeof(g_input_context.key_state));
    g_input_context.mouse_x = 0;
    g_input_context.mouse_y = 0;
    g_input_context.mouse_buttons = 0;
    
    // Clear message queue
    g_queue_head = 0;
    g_queue_tail = 0;
    
    g_input_context.initialized = 1;
    
    return 0;
}

void adapter_input_shutdown(void)
{
    SDL_StopTextInput();
    memset(&g_input_context, 0, sizeof(g_input_context));
    
    g_queue_head = 0;
    g_queue_tail = 0;
}

INPUT_CONTEXT* adapter_get_input_context(void)
{
    return &g_input_context;
}

// PROV: SDL main loop input functions for disc-less mode
// Event pump for main loop - processes all pending SDL events
void adapter_input_sdl_pump(void)
{
    // PROV: Main loop requires event processing for window/input management
    adapter_input_update();  // Use existing update function to process events
}

// PROV: ESC/close detection for main loop quit condition
int adapter_input_sdl_should_quit(void)
{
    // PROV: Main loop needs to detect quit conditions (ESC key or window close)
    
    // Check for queued quit messages
    if (g_queue_head != g_queue_tail) {
        for (uint32_t i = g_queue_head; i != g_queue_tail; i = (i + 1) % 256) {
            if (g_message_queue[i].message == WM_QUIT || g_message_queue[i].message == WM_CLOSE) {
                return 1;  // Found quit message
            }
        }
    }
    
    // Check for ESC key directly from SDL state
    const uint8_t *state = SDL_GetKeyboardState(NULL);
    if (state[SDL_SCANCODE_ESCAPE]) {
        return 1;  // ESC key pressed
    }
    
    return 0;  // No quit condition detected
}

// PROV: Key state detection for menu system
// Evidence: ENTER key detection required for title -> start skeleton transition
int adapter_input_sdl_is_key_pressed(int sdl_keycode)
{
    // PROV: Direct key state query for menu navigation
    const uint8_t *state = SDL_GetKeyboardState(NULL);
    
    // Convert SDL keycode to scancode for state array lookup
    SDL_Scancode scancode = SDL_GetScancodeFromKey(sdl_keycode);
    
    if (scancode != SDL_SCANCODE_UNKNOWN && scancode < SDL_NUM_SCANCODES) {
        return state[scancode] ? 1 : 0;
    }
    
    return 0;  // Key not found or invalid
}