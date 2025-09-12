#include "adapter_video.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// PROV: DirectDraw API replacements based on runtime.apis.json analysis
// Evidence: DirectDrawCreate at IAT_0x4c4cec, used for main graphics init
// Evidence: CreateWindowExA at IAT_0x4c4c10, creates 640x480 game window
// Evidence: GetDC/ReleaseDC for drawing operations on main window

static DDRAW_CONTEXT g_video_context = {0};

// PROV: VA_0x401000+ CreateWindowExA call sites from evidence
// Parameters: WS_OVERLAPPEDWINDOW style, 640x480 dimensions from BMP analysis  
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
    LPVOID lpParam)
{
    // PROV: Evidence shows 640x480 window creation for game display
    int width = (nWidth > 0) ? nWidth : ADAPTER_DEFAULT_WIDTH;
    int height = (nHeight > 0) ? nHeight : ADAPTER_DEFAULT_HEIGHT;
    
    // Initialize SDL video if not already done
    if (g_video_context.window == NULL) {
        if (SDL_Init(SDL_INIT_VIDEO) < 0) {
            fprintf(stderr, "SDL_Init failed: %s\n", SDL_GetError());
            return NULL;
        }
        
        // PROV: Game uses fullscreen-capable window based on evidence
        g_video_context.window = SDL_CreateWindow(
            lpWindowName ? lpWindowName : "SOTE",
            SDL_WINDOWPOS_CENTERED,
            SDL_WINDOWPOS_CENTERED,
            width, height,
            SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE
        );
        
        if (!g_video_context.window) {
            fprintf(stderr, "SDL_CreateWindow failed: %s\n", SDL_GetError());
            return NULL;
        }
        
        g_video_context.screen_width = width;
        g_video_context.screen_height = height;
        g_video_context.bpp = ADAPTER_DEFAULT_BPP;
    }
    
    return (HWND)g_video_context.window;
}

// PROV: ShowWindow call sites from runtime.apis.json
// Used for window visibility control during game state changes
BOOL adapter_ShowWindow(HWND hWnd, int nCmdShow)
{
    if (!g_video_context.window) return FALSE;
    
    switch (nCmdShow) {
        case SW_HIDE:
            SDL_HideWindow(g_video_context.window);
            break;
        case SW_SHOW:
        case SW_SHOWNORMAL:
            SDL_ShowWindow(g_video_context.window);
            break;
        case SW_SHOWMINIMIZED:
            SDL_MinimizeWindow(g_video_context.window);
            break;
        case SW_SHOWMAXIMIZED:
            SDL_MaximizeWindow(g_video_context.window);
            break;
        default:
            SDL_ShowWindow(g_video_context.window);
            break;
    }
    
    return TRUE;
}

// PROV: GetDC call sites at VA_0x401000+ for drawing operations
HDC adapter_GetDC(HWND hWnd)
{
    if (!g_video_context.window) return NULL;
    
    // Create renderer if not exists
    if (!g_video_context.renderer) {
        // PROV: Evidence suggests hardware acceleration preferred
        g_video_context.renderer = SDL_CreateRenderer(
            g_video_context.window, -1, 
            SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
        );
        
        if (!g_video_context.renderer) {
            // Fallback to software rendering
            g_video_context.renderer = SDL_CreateRenderer(
                g_video_context.window, -1, SDL_RENDERER_SOFTWARE
            );
        }
    }
    
    return (HDC)g_video_context.renderer;
}

// PROV: ReleaseDC call sites for resource cleanup
int adapter_ReleaseDC(HWND hWnd, HDC hDC)
{
    // SDL renderer cleanup handled by video_shutdown
    return 1;
}

// PROV: GetSystemMetrics call sites with SM_CXSCREEN/SM_CYSCREEN parameters
int adapter_GetSystemMetrics(int nIndex)
{
    SDL_DisplayMode mode;
    
    switch (nIndex) {
        case 0: // SM_CXSCREEN  
            if (SDL_GetCurrentDisplayMode(0, &mode) == 0) {
                return mode.w;
            }
            return 1024; // Default fallback
            
        case 1: // SM_CYSCREEN
            if (SDL_GetCurrentDisplayMode(0, &mode) == 0) {
                return mode.h;
            }
            return 768; // Default fallback
            
        case 32: // SM_CXFRAME
            return 8; // Default window frame width
            
        case 33: // SM_CYFRAME  
            return 8; // Default window frame height
            
        default:
            return 0;
    }
}

// PROV: DirectDrawCreate at IAT_0x4c4cec - main graphics initialization
HRESULT adapter_DirectDrawCreate(GUID *lpGUID, void **lplpDD, IUnknown *pUnkOuter)
{
    if (!lplpDD) return DDERR_INVALIDPARAMS;
    
    // Initialize video context
    if (adapter_video_init() != 0) {
        return DDERR_GENERIC;
    }
    
    // Return context as DirectDraw interface pointer
    *lplpDD = &g_video_context;
    g_video_context.initialized = 1;
    
    return DDERR_OK;
}

// PROV: DirectDrawEnumerateA at IAT_0x4c4cd4 - graphics device enumeration
HRESULT adapter_DirectDrawEnumerateA(void *lpCallback, LPVOID lpContext)
{
    // PROV: Evidence shows single graphics device usage
    // Return success indicating default device available
    return DDERR_OK;
}

// PROV: Surface operations for 43 BMP assets (640x480x24, 256x256x24)
DDRAW_SURFACE* adapter_CreateSurface(uint32_t width, uint32_t height, uint32_t bpp)
{
    if (!g_video_context.renderer) return NULL;
    
    DDRAW_SURFACE *surface = calloc(1, sizeof(DDRAW_SURFACE));
    if (!surface) return NULL;
    
    // PROV: BMP format analysis shows 24-bit RGB, some with 32-bit RGBA
    Uint32 format = (bpp == 32) ? SDL_PIXELFORMAT_RGBA8888 : SDL_PIXELFORMAT_RGB888;
    
    surface->texture = SDL_CreateTexture(
        g_video_context.renderer,
        format,
        SDL_TEXTUREACCESS_STREAMING,
        width, height
    );
    
    if (!surface->texture) {
        free(surface);
        return NULL;
    }
    
    surface->width = width;
    surface->height = height;
    surface->format = format;
    surface->pitch = width * (bpp / 8);
    
    return surface;
}

// PROV: BitBlt operations for bitmap rendering from evidence
HRESULT adapter_Blt(DDRAW_SURFACE *dest, RECT *destRect, DDRAW_SURFACE *src, RECT *srcRect, uint32_t flags)
{
    if (!dest || !src || !g_video_context.renderer) {
        return DDERR_INVALIDPARAMS;
    }
    
    SDL_Rect sdl_src = {0};
    SDL_Rect sdl_dest = {0};
    
    if (srcRect) {
        sdl_src.x = srcRect->left;
        sdl_src.y = srcRect->top;  
        sdl_src.w = srcRect->right - srcRect->left;
        sdl_src.h = srcRect->bottom - srcRect->top;
    } else {
        sdl_src.w = src->width;
        sdl_src.h = src->height;
    }
    
    if (destRect) {
        sdl_dest.x = destRect->left;
        sdl_dest.y = destRect->top;
        sdl_dest.w = destRect->right - destRect->left; 
        sdl_dest.h = destRect->bottom - destRect->top;
    } else {
        sdl_dest.w = dest->width;
        sdl_dest.h = dest->height;
    }
    
    if (SDL_RenderCopy(g_video_context.renderer, src->texture, &sdl_src, &sdl_dest) < 0) {
        return DDERR_GENERIC;
    }
    
    return DDERR_OK;
}

// PROV: Page flipping for double-buffered rendering
HRESULT adapter_Flip(DDRAW_SURFACE *surface, uint32_t flags)
{
    if (!g_video_context.renderer) return DDERR_GENERIC;
    
    SDL_RenderPresent(g_video_context.renderer);
    return DDERR_OK;
}

void adapter_FreeSurface(DDRAW_SURFACE *surface)
{
    if (surface) {
        if (surface->texture) {
            SDL_DestroyTexture(surface->texture);
        }
        if (surface->pixels) {
            free(surface->pixels);
        }
        free(surface);
    }
}

// Context management
int adapter_video_init(void)
{
    if (g_video_context.initialized) return 0;
    
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        return -1;
    }
    
    memset(&g_video_context, 0, sizeof(g_video_context));
    return 0;
}

void adapter_video_shutdown(void)
{
    if (g_video_context.renderer) {
        SDL_DestroyRenderer(g_video_context.renderer);
    }
    
    if (g_video_context.window) {
        SDL_DestroyWindow(g_video_context.window);
    }
    
    if (g_video_context.surfaces) {
        for (uint32_t i = 0; i < g_video_context.num_surfaces; i++) {
            adapter_FreeSurface((DDRAW_SURFACE*)g_video_context.surfaces[i]);
        }
        free(g_video_context.surfaces);
    }
    
    SDL_Quit();
    memset(&g_video_context, 0, sizeof(g_video_context));
}

DDRAW_CONTEXT* adapter_get_context(void)
{
    return &g_video_context;
}