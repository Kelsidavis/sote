#include "adapter_time.h"

// PROV: SDL2 compatibility header handles cross-platform includes
// No direct SDL includes needed - handled by adapter_time.h -> SDL_compat.h

// PROV: SDL stub functions for cross-compilation builds
#if (defined(WIN32_BUILD) && !defined(SOTE_FORCE_SDL)) || !defined(HAVE_ADAPTER_SDL)
static uint32_t SDL_GetTicks(void) { return 0; }
static uint64_t SDL_GetPerformanceCounter(void) { return 0; }
static uint64_t SDL_GetPerformanceFrequency(void) { return 1000; }  
static void SDL_Delay(uint32_t ms) { }
#endif

// PROV: Timing API replacements based on runtime.apis.json analysis
// Evidence: timeGetTime used for game loop timing and frame rate control

static uint64_t g_start_time = 0;
static uint64_t g_frequency = 0;

// PROV: timeGetTime call sites from evidence - main timing function for game loop
DWORD adapter_timeGetTime(void)
{
    // Return milliseconds since SDL initialization
    return SDL_GetTicks();
}

// High-resolution performance counter
BOOL adapter_QueryPerformanceCounter(LARGE_INTEGER *lpPerformanceCount)
{
    if (!lpPerformanceCount) return FALSE;
    
    lpPerformanceCount->QuadPart = SDL_GetPerformanceCounter();
    return TRUE;
}

BOOL adapter_QueryPerformanceFrequency(LARGE_INTEGER *lpFrequency)
{
    if (!lpFrequency) return FALSE;
    
    lpFrequency->QuadPart = SDL_GetPerformanceFrequency();
    return TRUE;
}

// Multimedia timer precision control
MMRESULT adapter_timeBeginPeriod(UINT uPeriod)
{
    // SDL handles timing precision internally
    return TIMERR_NOERROR;
}

MMRESULT adapter_timeEndPeriod(UINT uPeriod)
{
    // SDL handles timing precision internally
    return TIMERR_NOERROR;
}

// Utility functions
uint64_t adapter_get_performance_counter(void)
{
    return SDL_GetPerformanceCounter();
}

uint64_t adapter_get_performance_frequency(void)
{
    return SDL_GetPerformanceFrequency();
}

// Frame rate limiting
void adapter_limit_fps(uint32_t target_fps)
{
    static uint32_t last_time = 0;
    
    if (target_fps == 0) return;
    
    uint32_t target_ms = 1000 / target_fps;
    uint32_t current_time = SDL_GetTicks();
    uint32_t elapsed = current_time - last_time;
    
    if (elapsed < target_ms) {
        SDL_Delay(target_ms - elapsed);
    }
    
    last_time = SDL_GetTicks();
}

uint32_t adapter_get_fps(void)
{
    static uint32_t frame_count = 0;
    static uint32_t last_time = 0;
    static uint32_t current_fps = 0;
    
    frame_count++;
    uint32_t current_time = SDL_GetTicks();
    
    if (current_time - last_time >= 1000) {
        current_fps = frame_count;
        frame_count = 0;
        last_time = current_time;
    }
    
    return current_fps;
}

int adapter_time_init(void)
{
    g_start_time = SDL_GetPerformanceCounter();
    g_frequency = SDL_GetPerformanceFrequency();
    return 0;
}

void adapter_time_shutdown(void)
{
    g_start_time = 0;
    g_frequency = 0;
}