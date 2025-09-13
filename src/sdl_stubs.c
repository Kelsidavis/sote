/*
 * SDL2 Stub Implementation for SOTE Disc-less Runtime
 * PROV: Stub functions for demonstration when SDL2 MinGW libraries unavailable
 * NOTE: Provides minimal API compatibility for build completion
 */

#include <stdint.h>
#include <stdio.h>
#include <windows.h>

// SDL types and constants (minimal definitions)
typedef struct SDL_Surface {
    int w, h;
    void *pixels;
} SDL_Surface;

typedef struct SDL_Texture SDL_Texture;
typedef struct SDL_Renderer SDL_Renderer;
typedef void* SDL_Window;

#define SDL_INIT_VIDEO 0x00000020
#define SDL_WINDOWPOS_CENTERED 0x2FFF0000u

// SDL stub functions
uint32_t SDL_GetTicks(void) {
    return GetTickCount();
}

void SDL_Delay(uint32_t ms) {
    Sleep(ms);
}

int SDL_Init(uint32_t flags) {
    (void)flags;
    return 0;
}

void SDL_Quit(void) {
    // Stub
}

SDL_Window* SDL_CreateWindow(const char* title, int x, int y, int w, int h, uint32_t flags) {
    (void)title; (void)x; (void)y; (void)w; (void)h; (void)flags;
    return (SDL_Window*)1; // Non-null dummy pointer
}

void SDL_DestroyWindow(SDL_Window* window) {
    (void)window;
}

SDL_Renderer* SDL_CreateRenderer(SDL_Window* window, int index, uint32_t flags) {
    (void)window; (void)index; (void)flags;
    return (SDL_Renderer*)1; // Non-null dummy pointer
}

void SDL_DestroyRenderer(SDL_Renderer* renderer) {
    (void)renderer;
}

SDL_Surface* SDL_LoadBMP(const char* file) {
    (void)file;
    printf("[SDL_STUB] Would load BMP: %s\n", file ? file : "NULL");
    return NULL; // Indicate failure
}

SDL_Texture* SDL_CreateTextureFromSurface(SDL_Renderer* renderer, SDL_Surface* surface) {
    (void)renderer; (void)surface;
    return NULL; // Indicate failure
}

int SDL_RenderCopy(SDL_Renderer* renderer, SDL_Texture* texture, void* srcrect, void* dstrect) {
    (void)renderer; (void)texture; (void)srcrect; (void)dstrect;
    return 0; // Success
}

void SDL_RenderPresent(SDL_Renderer* renderer) {
    (void)renderer;
}

void SDL_DestroyTexture(SDL_Texture* texture) {
    (void)texture;
}

void SDL_FreeSurface(SDL_Surface* surface) {
    (void)surface;
}

const char* SDL_GetError(void) {
    return "SDL stub mode - operation not implemented";
}

int SDL_RenderClear(SDL_Renderer* renderer) {
    (void)renderer;
    return 0;
}

int SDL_SetRenderDrawColor(SDL_Renderer* renderer, uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
    (void)renderer; (void)r; (void)g; (void)b; (void)a;
    return 0;
}

// Input stubs
int SDL_PollEvent(void* event) {
    (void)event;
    return 0; // No events
}

const uint8_t* SDL_GetKeyboardState(int* numkeys) {
    (void)numkeys;
    static uint8_t stub_keys[256] = {0};
    return stub_keys;
}

uint32_t SDL_GetMouseState(int* x, int* y) {
    if (x) *x = 0;
    if (y) *y = 0;
    return 0;
}

void SDL_StartTextInput(void) {
    // Stub
}

void SDL_StopTextInput(void) {
    // Stub
}

// Performance counters
uint64_t SDL_GetPerformanceCounter(void) {
    return GetTickCount64();
}

uint64_t SDL_GetPerformanceFrequency(void) {
    return 1000; // 1ms resolution
}

// Additional SDL stubs needed for linking
void SDL_SetMainReady(void) {
    // Stub
}

SDL_Surface* SDL_GetWindowSurface(SDL_Window* window) {
    (void)window;
    return NULL;
}

int SDL_SaveBMP(SDL_Surface* surface, const char* file) {
    (void)surface;
    printf("[SDL_STUB] Would save BMP: %s\n", file ? file : "NULL");
    return 0;
}

int SDL_ShowWindow(SDL_Window* window) {
    (void)window;
    return 0;
}

int SDL_HideWindow(SDL_Window* window) {
    (void)window;
    return 0;
}

int SDL_MinimizeWindow(SDL_Window* window) {
    (void)window;
    return 0;
}

int SDL_MaximizeWindow(SDL_Window* window) {
    (void)window;
    return 0;
}

typedef struct SDL_DisplayMode {
    int w, h;
    int refresh_rate;
} SDL_DisplayMode;

int SDL_GetCurrentDisplayMode(int displayIndex, SDL_DisplayMode* mode) {
    (void)displayIndex;
    if (mode) {
        mode->w = 640;
        mode->h = 480;
        mode->refresh_rate = 60;
    }
    return 0;
}

SDL_Texture* SDL_CreateTexture(SDL_Renderer* renderer, uint32_t format, int access, int w, int h) {
    (void)renderer; (void)format; (void)access; (void)w; (void)h;
    return NULL;
}

const char* SDL_GetKeyName(int key) {
    (void)key;
    return "UNKNOWN";
}

// Initialization function called from main
void init_sdl2_stubs(void) {
    printf("[SDL_STUB] SDL2 stub mode initialized\n");
}