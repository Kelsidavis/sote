// PROV: SDL2 stub implementation for PE32 import table - RE-AGENT REBUILD m100
// Evidence: SDL2 documentation specifies these functions as essential for basic operation

#include <stdint.h>
#include <stdio.h>
#include <windows.h>

// Load SDL2.dll dynamically at runtime
static HMODULE g_sdl2_dll = NULL;

// Function pointer types for SDL2 functions
typedef int (*SDL_Init_t)(uint32_t flags);
typedef const char* (*SDL_GetError_t)(void);
typedef void (*SDL_SetMainReady_t)(void);
typedef void* (*SDL_CreateWindow_t)(const char* title, int x, int y, int w, int h, uint32_t flags);
typedef void (*SDL_DestroyWindow_t)(void* window);
typedef void* (*SDL_CreateRenderer_t)(void* window, int index, uint32_t flags);
typedef void (*SDL_DestroyRenderer_t)(void* renderer);
typedef void (*SDL_RenderPresent_t)(void* renderer);
typedef void (*SDL_Quit_t)(void);
typedef uint32_t (*SDL_GetTicks_t)(void);
typedef void (*SDL_Delay_t)(uint32_t ms);
typedef int (*SDL_PollEvent_t)(void* event);
typedef void* (*SDL_LoadBMP_t)(const char* file);
typedef void (*SDL_FreeSurface_t)(void* surface);
typedef int (*SDL_SaveBMP_t)(void* surface, const char* file);
typedef void* (*SDL_GetWindowSurface_t)(void* window);

// Global function pointers
static SDL_Init_t p_SDL_Init = NULL;
static SDL_GetError_t p_SDL_GetError = NULL;
static SDL_SetMainReady_t p_SDL_SetMainReady = NULL;
static SDL_CreateWindow_t p_SDL_CreateWindow = NULL;
static SDL_DestroyWindow_t p_SDL_DestroyWindow = NULL;
static SDL_CreateRenderer_t p_SDL_CreateRenderer = NULL;
static SDL_DestroyRenderer_t p_SDL_DestroyRenderer = NULL;
static SDL_RenderPresent_t p_SDL_RenderPresent = NULL;
static SDL_Quit_t p_SDL_Quit = NULL;
static SDL_GetTicks_t p_SDL_GetTicks = NULL;
static SDL_Delay_t p_SDL_Delay = NULL;
static SDL_PollEvent_t p_SDL_PollEvent = NULL;
static SDL_LoadBMP_t p_SDL_LoadBMP = NULL;
static SDL_FreeSurface_t p_SDL_FreeSurface = NULL;
static SDL_SaveBMP_t p_SDL_SaveBMP = NULL;
static SDL_GetWindowSurface_t p_SDL_GetWindowSurface = NULL;

// Initialize SDL2 dynamic loading
int init_sdl2_stubs(void) {
    g_sdl2_dll = LoadLibraryA("SDL2.dll");
    if (!g_sdl2_dll) {
        fprintf(stderr, "[SDL2_STUB] Failed to load SDL2.dll\n");
        return -1;
    }
    
    p_SDL_Init = (SDL_Init_t)GetProcAddress(g_sdl2_dll, "SDL_Init");
    p_SDL_GetError = (SDL_GetError_t)GetProcAddress(g_sdl2_dll, "SDL_GetError");
    p_SDL_SetMainReady = (SDL_SetMainReady_t)GetProcAddress(g_sdl2_dll, "SDL_SetMainReady");
    p_SDL_CreateWindow = (SDL_CreateWindow_t)GetProcAddress(g_sdl2_dll, "SDL_CreateWindow");
    p_SDL_DestroyWindow = (SDL_DestroyWindow_t)GetProcAddress(g_sdl2_dll, "SDL_DestroyWindow");
    p_SDL_CreateRenderer = (SDL_CreateRenderer_t)GetProcAddress(g_sdl2_dll, "SDL_CreateRenderer");
    p_SDL_DestroyRenderer = (SDL_DestroyRenderer_t)GetProcAddress(g_sdl2_dll, "SDL_DestroyRenderer");
    p_SDL_RenderPresent = (SDL_RenderPresent_t)GetProcAddress(g_sdl2_dll, "SDL_RenderPresent");
    p_SDL_Quit = (SDL_Quit_t)GetProcAddress(g_sdl2_dll, "SDL_Quit");
    p_SDL_GetTicks = (SDL_GetTicks_t)GetProcAddress(g_sdl2_dll, "SDL_GetTicks");
    p_SDL_Delay = (SDL_Delay_t)GetProcAddress(g_sdl2_dll, "SDL_Delay");
    p_SDL_PollEvent = (SDL_PollEvent_t)GetProcAddress(g_sdl2_dll, "SDL_PollEvent");
    p_SDL_LoadBMP = (SDL_LoadBMP_t)GetProcAddress(g_sdl2_dll, "SDL_LoadBMP");
    p_SDL_FreeSurface = (SDL_FreeSurface_t)GetProcAddress(g_sdl2_dll, "SDL_FreeSurface");
    p_SDL_SaveBMP = (SDL_SaveBMP_t)GetProcAddress(g_sdl2_dll, "SDL_SaveBMP");
    p_SDL_GetWindowSurface = (SDL_GetWindowSurface_t)GetProcAddress(g_sdl2_dll, "SDL_GetWindowSurface");
    
    return 0;
}

// Cleanup SDL2 dynamic loading
void cleanup_sdl2_stubs(void) {
    if (g_sdl2_dll) {
        FreeLibrary(g_sdl2_dll);
        g_sdl2_dll = NULL;
    }
}

// Stub implementations that forward to loaded SDL2.dll
int SDL_Init(uint32_t flags) {
    if (!p_SDL_Init) return -1;
    return p_SDL_Init(flags);
}

const char* SDL_GetError(void) {
    if (!p_SDL_GetError) return "SDL2.dll not loaded";
    return p_SDL_GetError();
}

void SDL_SetMainReady(void) {
    if (p_SDL_SetMainReady) p_SDL_SetMainReady();
}

void* SDL_CreateWindow(const char* title, int x, int y, int w, int h, uint32_t flags) {
    if (!p_SDL_CreateWindow) return NULL;
    return p_SDL_CreateWindow(title, x, y, w, h, flags);
}

void SDL_DestroyWindow(void* window) {
    if (p_SDL_DestroyWindow) p_SDL_DestroyWindow(window);
}

void* SDL_CreateRenderer(void* window, int index, uint32_t flags) {
    if (!p_SDL_CreateRenderer) return NULL;
    return p_SDL_CreateRenderer(window, index, flags);
}

void SDL_DestroyRenderer(void* renderer) {
    if (p_SDL_DestroyRenderer) p_SDL_DestroyRenderer(renderer);
}

void SDL_RenderPresent(void* renderer) {
    if (p_SDL_RenderPresent) p_SDL_RenderPresent(renderer);
}

void SDL_Quit(void) {
    if (p_SDL_Quit) p_SDL_Quit();
}

uint32_t SDL_GetTicks(void) {
    if (!p_SDL_GetTicks) return GetTickCount();
    return p_SDL_GetTicks();
}

void SDL_Delay(uint32_t ms) {
    if (p_SDL_Delay) {
        p_SDL_Delay(ms);
    } else {
        Sleep(ms);
    }
}

int SDL_PollEvent(void* event) {
    if (!p_SDL_PollEvent) return 0;
    return p_SDL_PollEvent(event);
}

void* SDL_LoadBMP(const char* file) {
    if (!p_SDL_LoadBMP) return NULL;
    return p_SDL_LoadBMP(file);
}

void SDL_FreeSurface(void* surface) {
    if (p_SDL_FreeSurface) p_SDL_FreeSurface(surface);
}

int SDL_SaveBMP(void* surface, const char* file) {
    if (!p_SDL_SaveBMP) return -1;
    return p_SDL_SaveBMP(surface, file);
}

void* SDL_GetWindowSurface(void* window) {
    if (!p_SDL_GetWindowSurface) return NULL;
    return p_SDL_GetWindowSurface(window);
}

// Additional functions needed by the adapter
void SDL_HideWindow(void* window) { /* stub */ (void)window; }
void SDL_ShowWindow(void* window) { /* stub */ (void)window; }
void SDL_MinimizeWindow(void* window) { /* stub */ (void)window; }
void SDL_MaximizeWindow(void* window) { /* stub */ (void)window; }
int SDL_GetCurrentDisplayMode(int displayIndex, void* mode) { /* stub */ (void)displayIndex; (void)mode; return 0; }
void* SDL_CreateTexture(void* renderer, uint32_t format, int access, int w, int h) { /* stub */ (void)renderer; (void)format; (void)access; (void)w; (void)h; return NULL; }
int SDL_RenderCopy(void* renderer, void* texture, const void* srcrect, const void* dstrect) { /* stub */ (void)renderer; (void)texture; (void)srcrect; (void)dstrect; return 0; }
void SDL_DestroyTexture(void* texture) { /* stub */ (void)texture; }
void* SDL_CreateTextureFromSurface(void* renderer, void* surface) { /* stub */ (void)renderer; (void)surface; return NULL; }
void SDL_SetRenderDrawColor(void* renderer, uint8_t r, uint8_t g, uint8_t b, uint8_t a) { /* stub */ (void)renderer; (void)r; (void)g; (void)b; (void)a; }
void SDL_RenderClear(void* renderer) { /* stub */ (void)renderer; }
const char* SDL_GetKeyName(int key) { /* stub */ (void)key; return "STUB_KEY"; }
int SDL_FillRect(void* dst, const void* rect, uint32_t color) { /* stub */ (void)dst; (void)rect; (void)color; return 0; }
const uint8_t* SDL_GetKeyboardState(int *numkeys) { /* stub */ (void)numkeys; static uint8_t keys[512] = {0}; return keys; }
uint32_t SDL_GetMouseState(int *x, int *y) { /* stub */ (void)x; (void)y; return 0; }
void SDL_StartTextInput(void) { /* stub */ }
void SDL_StopTextInput(void) { /* stub */ }
uint64_t SDL_GetPerformanceCounter(void) { /* stub */ return 0; }
uint64_t SDL_GetPerformanceFrequency(void) { /* stub */ return 1000; }