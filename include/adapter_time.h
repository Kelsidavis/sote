#ifndef ADAPTER_TIME_H
#define ADAPTER_TIME_H

#ifdef SOTE_FORCE_SDL
#include <SDL.h>  // Enabled for real SDL2 - RE-AGENT REBUILD m100
#endif
#include <stdint.h>
#include <windows.h>

// PROV: Evidence sources from runtime.apis.json
// timeGetTime: VA_0x401000+, used for game timing and frame rate control

// High-performance timing functions
// PROV: timeGetTime call sites from evidence - returns milliseconds since system start
DWORD adapter_timeGetTime(void);

// High-resolution performance counter (if needed)
BOOL adapter_QueryPerformanceCounter(LARGE_INTEGER *lpPerformanceCount);
BOOL adapter_QueryPerformanceFrequency(LARGE_INTEGER *lpFrequency);

// Timer functions for multimedia timing
MMRESULT adapter_timeBeginPeriod(UINT uPeriod);
MMRESULT adapter_timeEndPeriod(UINT uPeriod);

// Utility functions
int adapter_time_init(void);
void adapter_time_shutdown(void);

// Get high precision time for frame rate limiting
uint64_t adapter_get_performance_counter(void);
uint64_t adapter_get_performance_frequency(void);

// Frame timing utilities
void adapter_limit_fps(uint32_t target_fps);
uint32_t adapter_get_fps(void);

#endif // ADAPTER_TIME_H