#ifndef ADAPTER_VIDEO_H
#define ADAPTER_VIDEO_H

#ifdef SOTE_FORCE_SDL /* PROV: SDL2 include enabled for real SDL2 - RE-AGENT REBUILD m100 */
#include <SDL.h>
#else
// SDL stub types for cross-compilation
typedef struct SDL_Window SDL_Window;
typedef struct SDL_Renderer SDL_Renderer;
typedef struct SDL_Texture SDL_Texture;
#endif

#include <stdint.h>
#include <windows.h>

// PROV: Evidence sources from runtime.apis.json
// DirectDrawCreate: VA_0x401000+, IAT_0x4c4cec
// CreateWindowExA: VA_0x401000+, IAT_0x4c4c10
// GetDC/ReleaseDC: VA_0x401000+, IAT_0x4c4ba4/0x4c4b92

// DirectDraw interface simulation for SDL2
typedef struct {
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Texture **surfaces;
    uint32_t num_surfaces;
    uint32_t screen_width;
    uint32_t screen_height;
    uint32_t bpp;
    uint32_t initialized;
} DDRAW_CONTEXT;

typedef struct {
    SDL_Texture *texture;
    uint32_t width;
    uint32_t height;
    uint32_t pitch;
    void *pixels;
    uint32_t format;
    uint32_t locked;
} DDRAW_SURFACE;

// Window management
// PROV: CreateWindowExA call sites from evidence.curated.json
HWND adapter_CreateWindowExA(
    DWORD dwExStyle,
    LPCSTR lpClassName,
    LPCSTR lpWindowName,
    DWORD dwStyle,
    int X, int Y,
    int nWidth, int nHeight,
    HWND hWndParent,
    HMENU hMenu,
    HINSTANCE hInstance,
    LPVOID lpParam
);

// PROV: ShowWindow call sites from runtime.apis.json
BOOL adapter_ShowWindow(HWND hWnd, int nCmdShow);

// Device context management  
// PROV: GetDC call sites at VA_0x401000+ from IAT analysis
HDC adapter_GetDC(HWND hWnd);

// PROV: ReleaseDC call sites at VA_0x401000+ from IAT analysis  
int adapter_ReleaseDC(HWND hWnd, HDC hDC);

// System metrics
// PROV: GetSystemMetrics call sites, parameters SM_CXSCREEN/SM_CYSCREEN
int adapter_GetSystemMetrics(int nIndex);

// DirectDraw adapter functions
// PROV: DirectDrawCreate call sites at VA_0x401000+, IAT_0x4c4cec
HRESULT adapter_DirectDrawCreate(GUID *lpGUID, void **lplpDD, IUnknown *pUnkOuter);

// PROV: DirectDrawEnumerateA call sites at VA_0x401000+, IAT_0x4c4cd4  
HRESULT adapter_DirectDrawEnumerateA(void *lpCallback, LPVOID lpContext);

// Surface operations for bitmap rendering
// PROV: Expected from resource.catalog.json - 43 BMP files, 640x480x24 format
DDRAW_SURFACE* adapter_CreateSurface(uint32_t width, uint32_t height, uint32_t bpp);
HRESULT adapter_Blt(DDRAW_SURFACE *dest, RECT *destRect, DDRAW_SURFACE *src, RECT *srcRect, uint32_t flags);
HRESULT adapter_Flip(DDRAW_SURFACE *surface, uint32_t flags);
void adapter_FreeSurface(DDRAW_SURFACE *surface);

// Utility functions
int adapter_video_init(void);
void adapter_video_shutdown(void);
DDRAW_CONTEXT* adapter_get_context(void);

// PROV: SDL BMP splash rendering - coord-splash-002 + SDL main loop requirements
int adapter_video_sdl_init(int w, int h);  // PROV: Main loop init with dimensions
int adapter_video_sdl_present(void);       // PROV: Frame presentation for main loop  
int adapter_video_sdl_shutdown(void);      // PROV: Clean shutdown for main loop
int adapter_video_sdl_show_bmp(const char* bmp_path);  // PROV: BMP rendering function
int adapter_resolve_splash_bmp(char* resolved_path, size_t path_size);  // PROV: BMP path resolution from JSON

// PROV: Title scene helper APIs - coord-title-001
int adapter_video_sdl_clear(uint8_t r, uint8_t g, uint8_t b);  // PROV: Clear window with RGB color
int adapter_video_sdl_draw_bmp_centered(const char* bmp_path);  // PROV: Load and center draw BMP
int adapter_video_sdl_save_screenshot(const char* out_bmp_path);  // PROV: Screenshot capture for verification

// Constants from evidence
// PROV: Screen dimensions from resource analysis (640x480 BMP files)
#define ADAPTER_DEFAULT_WIDTH  640
#define ADAPTER_DEFAULT_HEIGHT 480
#define ADAPTER_DEFAULT_BPP    24

// Error codes matching DirectDraw HRESULT values
#define DDERR_OK                0x00000000
#define DDERR_GENERIC           0x80004005
#define DDERR_INVALIDPARAMS     0x80070057
#define DDERR_OUTOFMEMORY       0x8007000E
#define DDERR_SURFACEBUSY       0x88760168
#define DDERR_SURFACELOST       0x88760156

// Window show states from evidence
#define SW_HIDE         0
#define SW_SHOWNORMAL   1  
#define SW_SHOWMINIMIZED 2
#define SW_SHOWMAXIMIZED 3
#define SW_SHOW         5

#endif // ADAPTER_VIDEO_H