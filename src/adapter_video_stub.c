#include "../include/adapter_video.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Forward declaration for IUnknown stub
typedef struct IUnknown IUnknown;

// PROV: Minimal video adapter stub for cross-compilation
// Evidence: DirectDrawCreate at IAT_0x4c4cec, CreateWindowExA at IAT_0x4c4c10

static DDRAW_CONTEXT g_video_context = {0};

// Minimal SDL adapter implementations for cross-compilation
int adapter_video_init(void)
{
    if (g_video_context.initialized) return 0;
    
    printf("[ADAPTER] Video subsystem initialized (stub)\n");
    memset(&g_video_context, 0, sizeof(g_video_context));
    g_video_context.initialized = 1;
    g_video_context.screen_width = 640;
    g_video_context.screen_height = 480;
    g_video_context.bpp = 24;
    return 0;
}

void adapter_video_shutdown(void)
{
    printf("[ADAPTER] Video subsystem shutdown (stub)\n");
    memset(&g_video_context, 0, sizeof(g_video_context));
}

DDRAW_CONTEXT* adapter_get_context(void)
{
    return &g_video_context;
}

// Window management stubs
HWND adapter_CreateWindowExA(
    DWORD dwExStyle, LPCSTR lpClassName, LPCSTR lpWindowName,
    DWORD dwStyle, int X, int Y, int nWidth, int nHeight,
    HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam)
{
    (void)dwExStyle; (void)lpClassName; (void)lpWindowName; (void)dwStyle;
    (void)X; (void)Y; (void)hWndParent; (void)hMenu; (void)hInstance; (void)lpParam;
    printf("[ADAPTER] CreateWindow: %dx%d (stub)\n", nWidth, nHeight);
    return (HWND)0x12345678;
}

BOOL adapter_ShowWindow(HWND hWnd, int nCmdShow)
{
    (void)hWnd;
    printf("[ADAPTER] ShowWindow: mode=%d (stub)\n", nCmdShow);
    return TRUE;
}

HDC adapter_GetDC(HWND hWnd)
{
    (void)hWnd;
    printf("[ADAPTER] GetDC (stub)\n");
    return (HDC)0x87654321;
}

int adapter_ReleaseDC(HWND hWnd, HDC hDC)
{
    (void)hWnd; (void)hDC;
    printf("[ADAPTER] ReleaseDC (stub)\n");
    return 1;
}

int adapter_GetSystemMetrics(int nIndex)
{
    switch (nIndex) {
        case 0: return 640;   // SM_CXSCREEN  
        case 1: return 480;   // SM_CYSCREEN
        case 32: return 8;    // SM_CXFRAME
        case 33: return 8;    // SM_CYFRAME  
        default: return 0;
    }
}

// DirectDraw adapter stubs
HRESULT adapter_DirectDrawCreate(GUID *lpGUID, void **lplpDD, IUnknown *pUnkOuter)
{
    (void)lpGUID; (void)lplpDD; (void)pUnkOuter;
    printf("[ADAPTER] DirectDrawCreate (stub)\n");
    return 0; // S_OK
}

HRESULT adapter_DirectDrawEnumerateA(void *lpCallback, LPVOID lpContext)
{
    (void)lpCallback; (void)lpContext;
    printf("[ADAPTER] DirectDrawEnumerateA (stub)\n");
    return 0; // S_OK
}

// Surface operations stubs
DDRAW_SURFACE* adapter_CreateSurface(uint32_t width, uint32_t height, uint32_t bpp)
{
    printf("[ADAPTER] CreateSurface: %dx%dx%d (stub)\n", width, height, bpp);
    DDRAW_SURFACE *surface = calloc(1, sizeof(DDRAW_SURFACE));
    if (surface) {
        surface->width = width;
        surface->height = height;
        surface->format = bpp;
    }
    return surface;
}

HRESULT adapter_Blt(DDRAW_SURFACE *dest, RECT *destRect, DDRAW_SURFACE *src, RECT *srcRect, uint32_t flags)
{
    (void)dest; (void)destRect; (void)src; (void)srcRect; (void)flags;
    printf("[ADAPTER] Blt (stub)\n");
    return DDERR_OK;
}

HRESULT adapter_Flip(DDRAW_SURFACE *surface, uint32_t flags)
{
    (void)surface; (void)flags;
    printf("[ADAPTER] Flip (stub)\n");
    return DDERR_OK;
}

void adapter_FreeSurface(DDRAW_SURFACE *surface)
{
    if (surface) {
        printf("[ADAPTER] FreeSurface (stub)\n");
        free(surface);
    }
}