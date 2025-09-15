/*
 * RE-AGENT-BANNER
 * Agent: binary-reimplementation-engineer
 * Call ID: UNKNOWN
 * Artifact SHA256: be596ee755afbd4f3a50de366a07866d8dfed032f3341b63f539e5f93773ff77
 * Provenance: evidence.curated.json, layouts.curated.json, mappings.json
 */

/*
 * SOTE Entry Point Module
 * Reimplemented from binary analysis of SOTE.EXE
 * SHA256: be596ee755afbd4f3a50de366a07866d8dfed032f3341b63f539e5f93773ff77
 */

#define SDL_MAIN_HANDLED

#if !defined(_WIN32)
#ifndef SOTE_FORCE_SDL
#define SOTE_FORCE_SDL 1
#endif
#endif

#ifndef _WIN32
#define _GNU_SOURCE
#endif
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <strings.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include "../include/sote.h"
#include "../include/types.h"
#include "../include/adapter_video.h"
#include "../include/adapter_input.h"
#include "../include/adapter_audio.h"

// Audio adapter stubs for entry.c - using non-static to match header
#ifndef INCLUDE_ADAPTER_AUDIO_SDL_C
int adapter_audio_sdl_init(void) { return -1; }
void adapter_audio_sdl_shutdown(void) { }
#endif

#ifndef _WIN32
// Include comprehensive Windows compatibility layer for Linux builds
#include "../include/windows_compat.h"
// Include DirectDraw COM interface stubs for Linux compilation
#include "../include/windows_ddraw_compat.h"
// Additional missing APIs for entry.c
typedef char* LPCH;
static inline HANDLE GetCurrentProcess(void) { return (HANDLE)0; }
static inline HANDLE HeapCreate(DWORD flOptions, size_t dwInitialSize, size_t dwMaximumSize) { return (HANDLE)1; }
static inline DWORD SetErrorMode(DWORD uMode) { return uMode; }
static inline void* SetUnhandledExceptionFilter(void* lpTopLevelExceptionFilter) { return NULL; }
#define SEM_FAILCRITICALERRORS 0x0001
#define SEM_NOGPFAULTERRORBOX 0x0002
// Additional STARTUPINFO definition for compatibility
typedef struct {
    DWORD cb;
} STARTUPINFOA;
// Additional missing APIs
static inline DWORD GetTickCount(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (DWORD)(ts.tv_sec * 1000 + ts.tv_nsec / 1000000);
}
#endif

/*
 * Program entry point
 * Evidence: Offset 4279744, Size: 423
 * Original signature: entry0 ();
 * Stack frame: 156 bytes with 7 local variables
 * 
 * Stack layout analysis:
 * - var_70h (-112): 64-byte initialization buffer
 * - var_44h (-68): STARTUPINFO pointer
 * - var_40h (-64): Environment block
 * - var_28h (-40): Process handle
 * - var_18h (-24): Heap handle
 * - var_10h (-16): Initialization flags
 * - var_4h (-4): Result code
 */
void entry_point(void)
{
    // Local variables based on stack frame analysis
    char init_buffer[SOTE_MAX_BUFFER_SIZE];      // var_70h: Initialization buffer
    STARTUPINFOA startup_info;                   // var_44h: Startup information
    char** env_block;                            // var_40h: Environment variables
    HANDLE process_handle;                       // var_28h: Process handle
    HANDLE heap_handle;                          // var_18h: Heap handle
    DWORD flags;                                 // var_10h: Initialization flags
    int result;                                  // var_4h: Result code
    
    // Initialize variables
    memset(init_buffer, 0, sizeof(init_buffer));
    memset(&startup_info, 0, sizeof(startup_info));
    startup_info.cb = sizeof(startup_info);
    flags = 0;
    result = 0;
    
    // Get startup information
    // Evidence: GetStartupInfoA import detected
    GetStartupInfoA(&startup_info);
    
    // Get command line
    // Evidence: GetCommandLineA import detected
    LPSTR command_line = GetCommandLineA();
    
    // Get environment strings
    // Evidence: GetEnvironmentStrings import detected
    env_block = (char**)GetEnvironmentStrings();
    
    // Get current process handle
    // Evidence: GetCurrentProcess import detected
    process_handle = GetCurrentProcess();
    
    // Initialize heap
    // PROV: HeapCreate @ IAT (evidence.curated.json)
    // NOTE: GetProcessHeap NOT in imports - use HeapCreate directly
    heap_handle = HeapCreate(0, 0, 0);
    
    // Set error mode
    // Evidence: SetErrorMode import detected
    SetErrorMode(SEM_FAILCRITICALERRORS | SEM_NOGPFAULTERRORBOX);
    
    // Initialize critical sections
    // Evidence: InitializeCriticalSection import detected
    // Note: Actual critical section would be in global data
    
    // Set unhandled exception filter
    // Evidence: SetUnhandledExceptionFilter import detected
    SetUnhandledExceptionFilter(NULL);
    
    // Parse command line into argc/argv format
    // PROV: Command line parsing inferred from GetCommandLineA @ IAT 0x4721920
    // TODO_EVID("actual parsing at entry0 needs decompilation @ 0x004159c0")
    int argc = 0;
    char** argv = NULL;
    
    if (command_line && *command_line) {
        // PROV: Argument counting pattern typical of Win32 apps
        // TODO_EVID("exact parsing logic from decompiler @ 0x004159c0+104")
        char* temp = command_line;
        BOOL in_quotes = FALSE;
        BOOL in_arg = FALSE;
        
        while (*temp) {
            if (*temp == '"') {
                in_quotes = !in_quotes;
            } else if (!in_quotes && (*temp == ' ' || *temp == '\t')) {
                if (in_arg) {
                    argc++;
                    in_arg = FALSE;
                }
            } else if (!in_arg) {
                in_arg = TRUE;
            }
            temp++;
        }
        if (in_arg) argc++;
        
        // Allocate argv array
        // PROV: HeapAlloc @ IAT 0x4721936 (evidence.curated.json)
        if (argc > 0) {
            argv = (char**)HeapAlloc(heap_handle, HEAP_ZERO_MEMORY, sizeof(char*) * (argc + 1));
            if (argv) {
                // Parse arguments into array
                int arg_index = 0;
                temp = command_line;
                in_quotes = FALSE;
                in_arg = FALSE;
                char* arg_start = NULL;
                
                while (*temp && arg_index < argc) {
                    if (*temp == '"') {
                        in_quotes = !in_quotes;
                        if (!in_arg) {
                            arg_start = temp + 1;
                            in_arg = TRUE;
                        }
                    } else if (!in_quotes && (*temp == ' ' || *temp == '\t')) {
                        if (in_arg) {
                            int arg_len = (int)(temp - arg_start);
                            argv[arg_index] = (char*)HeapAlloc(heap_handle, 0, arg_len + 1);
                            if (argv[arg_index]) {
                                strncpy(argv[arg_index], arg_start, arg_len);
                                argv[arg_index][arg_len] = '\0';
                            }
                            arg_index++;
                            in_arg = FALSE;
                        }
                    } else if (!in_arg) {
                        arg_start = temp;
                        in_arg = TRUE;
                    }
                    temp++;
                }
                
                // Handle last argument
                if (in_arg && arg_index < argc) {
                    int arg_len = (int)(temp - arg_start);
                    argv[arg_index] = (char*)HeapAlloc(heap_handle, 0, arg_len + 1);
                    if (argv[arg_index]) {
                        strncpy(argv[arg_index], arg_start, arg_len);
                        argv[arg_index][arg_len] = '\0';
                    }
                }
            }
        }
    }
    
    // PROV: Insert ultra-early guard for startup fault addresses 0x0041007e, 0x0041004a
    // PROV: Guard justified by startup_fault.symbols.json analysis - data structure access before init
    if (!argv) {
        #ifdef RESOURCE_WARNINGS
        fprintf(stderr, "[STARTUP] GUARD: argv is NULL - preventing data fault\n");
        #endif
        ExitProcess(1);
    }
    
    if (!env_block) {
        #ifdef RESOURCE_WARNINGS
        fprintf(stderr, "[STARTUP] GUARD: env_block is NULL - preventing data fault\n");
        #endif
        ExitProcess(1);
    }

    // Call main function - SDL2 signature requires (argc, argv)
    // PROV: Call to main @ 0x0042d63e (offset 4377150) from evidence.curated.json  
    // NOTE: SDL2 main redefined to SDL_main(int argc, char *argv[])
    result = main(argc, argv);
    
    // Cleanup
    // PROV: HeapFree @ IAT 0x4721928 (evidence.curated.json)
    if (argv) {
        for (int i = 0; i < argc; i++) {
            if (argv[i]) {
                HeapFree(heap_handle, 0, argv[i]);
            }
        }
        HeapFree(heap_handle, 0, argv);
    }
    
    if (env_block) {
        // PROV: FreeEnvironmentStringsA @ IAT 0x4721988 (evidence.curated.json)
        FreeEnvironmentStringsA((LPCH)env_block);
    }
    
    // Exit with result code
    // PROV: ExitProcess @ IAT 0x4722124 (evidence.curated.json)
    ExitProcess(result);
}

/*
 * OpenGL Environment Detection Helper - coord-gl-001
 * PROV: Environment gate for forcing OpenGL path over DirectDraw/SDL
 * Evidence: OpenGL video path extension for SOTE
 */
static int sote_should_use_opengl(void)
{
    // PROV: Check SOTE_FORCE_GL environment variable first
    const char *force_gl = getenv("SOTE_FORCE_GL");
    if (force_gl && (strcmp(force_gl, "1") == 0 || strcasecmp(force_gl, "true") == 0)) {
        #ifdef RESOURCE_WARNINGS
        fprintf(stderr, "[ROUTE] SOTE_FORCE_GL=1 detected - forcing OpenGL path\n");
        #endif
        return 1;
    }

    return 0;
}

/*
 * SDL Environment Detection Helper - coord-sdl-splash-001
 * PROV: Environment gate for forcing SDL path over DirectDraw
 * Evidence: Environment variables from coordinated disc-less requirements
 */
static int sote_should_use_sdl(void)
{
    // PROV: OpenGL takes priority over SDL if both are set
    if (sote_should_use_opengl()) {
        return 0;
    }

    // PROV: Check SOTE_FORCE_SDL environment variable first
    const char *force_sdl = getenv("SOTE_FORCE_SDL");
    if (force_sdl && (strcmp(force_sdl, "1") == 0 || strcasecmp(force_sdl, "true") == 0)) {
        #ifdef RESOURCE_WARNINGS
        fprintf(stderr, "[ROUTE] SOTE_FORCE_SDL=1 detected - forcing SDL path\n");
        #endif
        return 1;
    }

    // PROV: Check SOTE_DISCLESS environment variable as secondary gate
    const char *discless = getenv("SOTE_DISCLESS");
    if (discless && (strcmp(discless, "1") == 0 || strcasecmp(discless, "true") == 0)) {
        #ifdef RESOURCE_WARNINGS
        fprintf(stderr, "[ROUTE] SOTE_DISCLESS=1 detected - enabling SDL path\n");
        #endif
        return 1;
    }

    #ifdef SOTE_DISCLESS
    // PROV: Compile-time disc-less mode forces SDL
    #ifdef RESOURCE_WARNINGS
    fprintf(stderr, "[ROUTE] Compile-time SOTE_DISCLESS active - forcing SDL\n");
    #endif
    return 1;
    #endif

    return 0;
}

/*
 * Video subsystem initialization with SDL adapter guards
 * PROV: Guard added per Wine crash triage and disc-less requirements
 */

HRESULT initialize_video_subsystem(GraphicsContext* ctx)
{
    if (!ctx) {
        return E_INVALIDARG;
    }
    
    // PROV: Check for OpenGL path first (Linux and Windows) - coord-gl-001
    if (sote_should_use_opengl()) {
        #ifdef RESOURCE_WARNINGS
        fprintf(stderr, "[ROUTE] OpenGL video path (SDL2 + GL)\n");
        fprintf(stderr, "[ROUTE] selecting OpenGL video adapter\n");
        #endif

        // PROV: Initialize OpenGL video adapter with standard dimensions (640x480)
        extern int adapter_video_opengl_init(int w, int h);
        extern void adapter_video_opengl_shutdown(void);
        extern int adapter_video_opengl_show_bmp(const char* bmp_path);
        extern int adapter_video_opengl_clear(uint8_t r, uint8_t g, uint8_t b);

        int gl_video_result = adapter_video_opengl_init(640, 480);
        if (gl_video_result == 0) {
            #ifdef RESOURCE_WARNINGS
            fprintf(stderr, "[ERROR] OpenGL video init failed\n");
            #endif
            return E_FAIL;
        }

        #ifdef RESOURCE_WARNINGS
        fprintf(stderr, "[STARTUP] OpenGL video path initialized\n");
        #endif

        // PROV: Simple OpenGL demo - clear and show splash
        adapter_video_opengl_clear(0, 0, 0);  // Clear to black

        const char *assets_dir = getenv("SOTE_ASSETS_DIR");
        if (!assets_dir) assets_dir = "Sdata";

        char splash_path[512];
        snprintf(splash_path, sizeof(splash_path), "%s/boba.bmp", assets_dir);
        adapter_video_opengl_show_bmp(splash_path);

        // PROV: Brief delay then return success
        #ifndef _WIN32
        usleep(1000000); // 1 second delay
        #else
        Sleep(1000);
        #endif

        return S_OK;
    }

    // PROV: Guard justified by Wine backtrace + DD NULL deref.
    #if defined(SOTE_DISCLESS) || defined(HAVE_ADAPTER_SDL)
    // PROV: Disc-less/SDL path; skip DirectDraw per prior crash triage.
    // TODO_EVID("Original DD path retained under non-disc-less builds.")
    printf("[DISCLESS] Video adapter: SDL\n");
    return adapter_video_init();
    #else
    // Original DirectDraw path for non-disc-less builds
    if (getenv("SOTE_FORCE_SDL")) {
        return 0;
    } else {
        return initialize_directdraw(ctx);
    }
    #endif
}

/*
 * DirectX helper functions implementation
 * Evidence: DirectDraw imports (DirectDrawCreate, DirectDrawEnumerateA)
 */

HRESULT initialize_directdraw(GraphicsContext* ctx)
{
    // PROV: Check for OpenGL path first (cross-platform) - coord-gl-001
    if (sote_should_use_opengl()) {
        #ifdef RESOURCE_WARNINGS
        fprintf(stderr, "[ROUTE] OpenGL video path (SDL2 + GL)\n");
        fprintf(stderr, "[ROUTE] selecting OpenGL video adapter\n");
        #endif

        // PROV: Initialize OpenGL video adapter with standard dimensions (640x480)
        extern int adapter_video_opengl_init(int w, int h);
        extern void adapter_video_opengl_shutdown(void);
        extern int adapter_video_opengl_show_bmp(const char* bmp_path);
        extern int adapter_video_opengl_clear(uint8_t r, uint8_t g, uint8_t b);

        int gl_video_result = adapter_video_opengl_init(640, 480);
        if (gl_video_result == 0) {
            #ifdef RESOURCE_WARNINGS
            fprintf(stderr, "[ERROR] OpenGL video init failed\n");
            #endif
            return E_FAIL;
        }

        #ifdef RESOURCE_WARNINGS
        fprintf(stderr, "[STARTUP] OpenGL video path initialized\n");
        #endif

        // PROV: Simple OpenGL demo - clear and show splash
        adapter_video_opengl_clear(0, 0, 0);  // Clear to black

        const char *assets_dir = getenv("SOTE_ASSETS_DIR");
        if (!assets_dir) assets_dir = "Sdata";

        char splash_path[512];
        snprintf(splash_path, sizeof(splash_path), "%s/boba.bmp", assets_dir);
        adapter_video_opengl_show_bmp(splash_path);

        // PROV: Brief delay then return success
        #ifndef _WIN32
        usleep(1000000); // 1 second delay
        #else
        Sleep(1000);
        #endif

        return S_OK;
    }

#if !defined(_WIN32) || defined(SOTE_FORCE_SDL)
    fprintf(stderr, "[ROUTE] DDraw disabled on this build/route — skipping initialize_directdraw()\n");
    return 0; /* SDL path handles video; DDraw is bypassed. */
#endif
    // PROV: Early route decision logging and SDL gate check - coord-sdl-splash-001
    #ifdef RESOURCE_WARNINGS
    fprintf(stderr, "[STARTUP] initialize_directdraw entry - checking routing gates\n");
    #endif

    // PROV: Early OpenGL route check with short-circuit return - coord-gl-001
    if (sote_should_use_opengl()) {
        #ifdef RESOURCE_WARNINGS
        fprintf(stderr, "[ROUTE] OpenGL video path (SDL2 + GL)\n");
        fprintf(stderr, "[ROUTE] selecting OpenGL video adapter\n");
        #endif

        // PROV: Initialize OpenGL video adapter with standard dimensions (640x480)
        extern int adapter_video_opengl_init(int w, int h);
        extern void adapter_video_opengl_shutdown(void);
        extern int adapter_video_opengl_show_bmp(const char* bmp_path);
        extern int adapter_video_opengl_clear(uint8_t r, uint8_t g, uint8_t b);

        int gl_video_result = adapter_video_opengl_init(640, 480);
        if (gl_video_result == 0) {
            #ifdef RESOURCE_WARNINGS
            fprintf(stderr, "[ERROR] OpenGL video init failed\n");
            #endif
            return E_FAIL;
        }

        #ifdef RESOURCE_WARNINGS
        fprintf(stderr, "[STARTUP] OpenGL video path initialized\n");
        #endif

        // PROV: Simple OpenGL demo - clear and show splash
        adapter_video_opengl_clear(0, 0, 0);  // Clear to black

        const char *assets_dir = getenv("SOTE_ASSETS_DIR");
        if (!assets_dir) assets_dir = "Sdata";

        char splash_path[512];
        snprintf(splash_path, sizeof(splash_path), "%s/boba.bmp", assets_dir);
        adapter_video_opengl_show_bmp(splash_path);

        // PROV: Brief delay then return success
        #ifndef _WIN32
        usleep(1000000); // 1 second delay
        #else
        Sleep(1000);
        #endif

        return S_OK;
    }

    // PROV: Early SDL route check with short-circuit return
    if (sote_should_use_sdl()) {
        #ifdef RESOURCE_WARNINGS
        fprintf(stderr, "[ROUTE] forcing SDL path (disc-less)\n");
        #endif
        
        // PROV: Complete SDL main loop implementation for disc-less mode
        #ifdef RESOURCE_WARNINGS
        fprintf(stderr, "[STARTUP] disc-less SDL loop begin\n");
        #endif
        
        // PROV: Initialize SDL video adapter with standard dimensions (640x480)
        int sdl_video_result = adapter_video_sdl_init(640, 480);
        if (sdl_video_result != 0) {
            #ifdef RESOURCE_WARNINGS
            fprintf(stderr, "[ERROR] SDL video init failed: %d\n", sdl_video_result);
            #endif
            return E_FAIL;
        }
        
        // PROV: Optional audio initialization (non-fatal if fails)
        int sdl_audio_result = adapter_audio_sdl_init();
        if (sdl_audio_result != 0) {
            #ifdef RESOURCE_WARNINGS
            fprintf(stderr, "[AUDIO] SDL audio init failed, continuing without audio\n");
            #endif
            // TODO_EVID: Audio failure is non-fatal in disc-less mode
        }
        
        // PROV: Enhanced splash to title scene progression - coord-title-003
        // Evidence: title_frame.selection.json specifies boba.bmp fallback and timing requirements
        const char *assets_dir = getenv("SOTE_ASSETS_DIR");
        if (!assets_dir) assets_dir = "Sdata";  // PROV: Default assets path for VFS resolution
        
        // PROV: Splash phase configuration (800ms with boba.bmp) - VFS resolution paths
        char splash_bmp_path[512];
        snprintf(splash_bmp_path, sizeof(splash_bmp_path), "boba.bmp");  // PROV: Use VFS relative path
        
        // PROV: Title phase configuration from selection JSON - VFS resolution paths
        char title_bmp_path[512];  
        snprintf(title_bmp_path, sizeof(title_bmp_path), "boba.bmp");  // PROV: Use VFS relative path
        
        // PROV: Three-phase timing configuration (splash: 800ms, title: 5000ms, start_skeleton: 2000ms, 60fps)
        uint32_t loop_start = SDL_GetTicks();
        uint32_t splash_duration = 800;   // 800ms splash phase
        uint32_t title_duration = 5000;   // 5000ms title phase
        uint32_t start_duration = 2000;   // 2000ms start skeleton phase
        uint32_t frame_time = 1000 / 60;  // 60 FPS timing guard
        uint32_t last_frame = loop_start;
        int frame_count = 0;
        int current_phase = 0;  // 0=splash, 1=title, 2=start_skeleton
        int enter_pressed = 0;  // PROV: Track ENTER key for menu selection
        
        #ifdef RESOURCE_WARNINGS
        fprintf(stderr, "[LOOP] Enhanced SDL title sequence start, splash=%dms, title=%dms\n", 
                splash_duration, title_duration);
        #endif
        
        // PROV: Three-phase SDL event loop (splash + title + start_skeleton sequence)
        while (1) {
            uint32_t current_time = SDL_GetTicks();
            uint32_t elapsed = current_time - loop_start;
            
            // PROV: Phase transition logic
            if (current_phase == 0 && elapsed >= splash_duration) {
                current_phase = 1;  // Transition to title phase
                #ifdef RESOURCE_WARNINGS
                fprintf(stderr, "[ROUTE] title phase using: %s\n", title_bmp_path);
                #endif
            } else if (current_phase == 1 && enter_pressed) {
                // PROV: ENTER key pressed during title phase - transition to start skeleton
                current_phase = 2;
                #ifdef RESOURCE_WARNINGS
                fprintf(stderr, "[MENU] start selected\n");
                #endif
                
                // PROV: Capture title frame before transition
                adapter_video_sdl_save_screenshot("/media/k/vbox1/Shadows/SOTE/reports/runtime/title_frame.bmp");
                
                // Reset elapsed timer for start skeleton phase
                loop_start = current_time;
                elapsed = 0;
            }
            
            // PROV: Complete runtime limit check based on current phase
            if (current_phase == 2 && elapsed >= start_duration) {
                #ifdef RESOURCE_WARNINGS
                fprintf(stderr, "[LOOP] Start skeleton phase complete: %dms\n", elapsed);
                #endif
                break;
            } else if (current_phase == 1 && elapsed >= (splash_duration + title_duration)) {
                #ifdef RESOURCE_WARNINGS
                fprintf(stderr, "[LOOP] Title phase timeout reached: %dms\n", elapsed);
                #endif
                break;
            }
            
            // PROV: Input pump and quit detection
            adapter_input_sdl_pump();
            if (adapter_input_sdl_should_quit()) {
                #ifdef RESOURCE_WARNINGS
                fprintf(stderr, "[LOOP] Quit detected (ESC or close)\n");
                #endif
                break;
            }
            
            // PROV: Check for ENTER key during title phase for menu selection
            if (current_phase == 1 && !enter_pressed) {
                if (adapter_input_sdl_is_key_pressed(SDLK_RETURN)) {
                    enter_pressed = 1;
                    #ifdef RESOURCE_WARNINGS
                    fprintf(stderr, "[INPUT] ENTER key detected in title phase\n");
                    #endif
                }
            }
            
            // PROV: Frame timing control (60 FPS target)
            if (current_time - last_frame >= frame_time) {
                // PROV: Clear screen before rendering
                adapter_video_sdl_clear(0, 0, 0);  // Black background
                
                // PROV: Phase-specific rendering
                if (current_phase == 0) {
                    // Splash phase - render splash BMP
                    if (adapter_video_sdl_draw_bmp_centered(splash_bmp_path) == 0 && frame_count == 0) {
                        #ifdef RESOURCE_WARNINGS
                        fprintf(stderr, "[RENDER] splash presented: %s\n", 
                                strrchr(splash_bmp_path, '/') ? strrchr(splash_bmp_path, '/') + 1 : splash_bmp_path);
                        #endif
                        
                        // PROV: Save splash frame artifact
                        adapter_video_sdl_save_screenshot("SOTE/reports/runtime/splash_frame.bmp");
                    }
                    
                    // TODO_EVID: SAN playback codec unimplemented in diskless mode (resources.manifest.json shows 17 SAN files)
                    // PROV: Graceful skip for unsupported SAN multimedia codec in resource-faithful runtime
                    
                    // PROV: Log transition to title when entering phase 1 next iteration
                    if (elapsed >= (splash_duration - frame_time)) {
                        #ifdef RESOURCE_WARNINGS
                        fprintf(stderr, "[ROUTE] splash phase complete, transitioning to title\n");
                        #endif
                    }
                } else if (current_phase == 1) {
                    // Title phase - render title BMP 
                    if (adapter_video_sdl_draw_bmp_centered(title_bmp_path) == 0) {
                        // PROV: Save title frame artifact on first title render
                        static int title_captured = 0;
                        if (!title_captured) {
                            #ifdef RESOURCE_WARNINGS
                            fprintf(stderr, "[RENDER] title presented: %s\n", 
                                    strrchr(title_bmp_path, '/') ? strrchr(title_bmp_path, '/') + 1 : title_bmp_path);
                            #endif
                            
                            adapter_video_sdl_save_screenshot("/media/k/vbox1/Shadows/SOTE/reports/runtime/title_frame_initial.bmp");
                            title_captured = 1;
                        }
                    }
                } else if (current_phase == 2) {
                    // PROV: Start skeleton phase - render evidenced BMP placeholder
                    // Using boba.bmp as placeholder since it's an existing asset (evidence-based)
                    if (adapter_video_sdl_draw_bmp_centered("boba.bmp") == 0) {
                        // PROV: Save start frame artifact on first render
                        static int start_captured = 0;
                        if (!start_captured) {
                            #ifdef RESOURCE_WARNINGS
                            fprintf(stderr, "[RENDER] start skeleton presented: boba.bmp (placeholder)\n");
                            #endif
                            
                            adapter_video_sdl_save_screenshot("/media/k/vbox1/Shadows/SOTE/reports/runtime/start_frame.bmp");
                            start_captured = 1;
                        }
                    }
                }
                
                // PROV: Present frame to display
                adapter_video_sdl_present();
                
                last_frame = current_time;
                frame_count++;
            }
            
            // Small delay to prevent busy waiting
            SDL_Delay(1);
        }
        
        #ifdef RESOURCE_WARNINGS
        fprintf(stderr, "[LOOP] Enhanced SDL title sequence completed, frames=%d\n", frame_count);
        #endif
        
        // PROV: Complete shutdown sequence
        adapter_audio_sdl_shutdown();
        adapter_video_sdl_shutdown();
        
        #ifdef RESOURCE_WARNINGS
        fprintf(stderr, "[SHUTDOWN] disc-less SDL loop end\n");
        #endif
        
        return S_OK;
    }
    
    // PROV: Continue with DirectDraw path if SDL not forced
    #ifdef RESOURCE_WARNINGS
    fprintf(stderr, "[ROUTE] DirectDraw path selected\n");
    #endif
    
    if (!ctx) {
        return E_INVALIDARG;
    }
    
    /* [RESOURCE:VIDEO] DirectDrawCreate
     * PROV: va=0x401000+, iat=0x4c4cec, src=imports.DDRAW.dll
     * NOTE: Initialize DirectDraw subsystem for hardware-accelerated graphics
     * RESOURCE: Primary display adapter
     */
    #ifdef SOTE_DISCLESS
    // Stub DirectDraw for disc-less mode
    printf("[DISCLESS] DirectDraw stubbed - disc-less mode\n");
    ctx->dd_object = NULL;
    HRESULT hr = 0; // S_OK
    #else
    HRESULT hr = DirectDrawCreate(NULL, &ctx->dd_object, NULL);
    #endif
    if (FAILED(hr)) {
        return hr;
    }
    
    // PROV: Guard against NULL DirectDraw interface (crash site symbol VA 0x00404240, evidence from backtrace and map files)
    // PROV: Crash at initialize_directdraw+0x20 due to NULL dereference under Wine
    if (!ctx->dd_object) {
        goto dd_unavailable;
    }
    
    // PROV: Guard lpVtbl pointer access (crash site evidence from winedbg.backtrace.symbolized.txt)
    if (!ctx->dd_object->lpVtbl) {
        goto dd_unavailable;
    }
    
    // Set display mode (common for games)
    hr = ctx->dd_object->lpVtbl->SetDisplayMode(ctx->dd_object, 640, 480, 8);
    if (FAILED(hr)) {
        ctx->dd_object->lpVtbl->Release(ctx->dd_object);
        ctx->dd_object = NULL;
        return hr;
    }
    
    // Store display settings
    ctx->screen_width = 640;
    ctx->screen_height = 480;
    ctx->color_depth = 8;
    
    // Create surfaces
    return create_surfaces(ctx);

dd_unavailable:
    // PROV: Disc-less fallback entered because DirectDraw interface is unavailable under Wine.
    // TODO_EVID("Original DD init may differ; this branch preserves startup in disc-less mode.")
    #ifdef RESOURCE_WARNINGS
    fprintf(stderr, "[DISCLESS] DirectDraw unavailable — continuing with SDL/adapter path.\n");
    #endif
    #ifdef HAVE_ADAPTER_DDRAW
    adapter_ddraw_init_stub(); // PROV: stub only; does not assert behavior.
    #endif
    return 0; // PROV: Return chosen to avoid crash; see verification plan.
}

void shutdown_directdraw(GraphicsContext* ctx)
{
    if (!ctx) {
        return;
    }
    
    if (ctx->palette) {
        ctx->palette->lpVtbl->Release(ctx->palette);
        ctx->palette = NULL;
    }
    
    if (ctx->back_buffer) {
        ctx->back_buffer->lpVtbl->Release(ctx->back_buffer);
        ctx->back_buffer = NULL;
    }
    
    if (ctx->primary_surface) {
        ctx->primary_surface->lpVtbl->Release(ctx->primary_surface);
        ctx->primary_surface = NULL;
    }
    
    if (ctx->dd_object) {
        ctx->dd_object->lpVtbl->Release(ctx->dd_object);
        ctx->dd_object = NULL;
    }
}

HRESULT create_surfaces(GraphicsContext* ctx)
{
    if (!ctx) {
        return E_INVALIDARG;
    }
    
    // PROV: Handle NULL DirectDraw object for disc-less mode (crash prevention)
    if (!ctx->dd_object) {
        // PROV: Return success for disc-less mode without surfaces
        ctx->primary_surface = NULL;
        ctx->back_buffer = NULL;
        return S_OK;
    }
    
    // Create primary surface
    DDSURFACEDESC ddsd;
    memset(&ddsd, 0, sizeof(ddsd));
    ddsd.dwSize = sizeof(ddsd);
    ddsd.dwFlags = DDSD_CAPS;
    ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;
    
    HRESULT hr = ctx->dd_object->lpVtbl->CreateSurface(ctx->dd_object, &ddsd, &ctx->primary_surface, NULL);
    if (FAILED(hr)) {
        return hr;
    }
    
    return S_OK;
}

/*
 * Configuration functions implementation
 * Evidence: Registry imports (RegOpenKeyA, RegQueryValueExA, etc.)
 */

BOOL load_config(ConfigData* config)
{
    if (!config) {
        return FALSE;
    }
    
    // Initialize with defaults
    config->graphics_quality = 1;
    config->sound_enabled = TRUE;
    config->fullscreen_mode = FALSE;
    strcpy(config->save_path, ".");
    
    // Load from registry
    HKEY key;
    LONG result = RegOpenKeyExA(HKEY_CURRENT_USER, "Software\\SOTE", 0, KEY_READ, &key);
    if (result == ERROR_SUCCESS) {
        DWORD size = sizeof(DWORD);
        RegQueryValueExA(key, "GraphicsQuality", NULL, NULL, (BYTE*)&config->graphics_quality, &size);
        RegQueryValueExA(key, "SoundEnabled", NULL, NULL, (BYTE*)&config->sound_enabled, &size);
        RegQueryValueExA(key, "FullscreenMode", NULL, NULL, (BYTE*)&config->fullscreen_mode, &size);
        
        size = MAX_PATH;
        RegQueryValueExA(key, "SavePath", NULL, NULL, (BYTE*)config->save_path, &size);
        
        RegCloseKey(key);
        return TRUE;
    }
    
    return FALSE;
}

BOOL save_config(const ConfigData* config)
{
    if (!config) {
        return FALSE;
    }
    
    HKEY key;
    DWORD disposition;
    LONG result = RegCreateKeyExA(HKEY_CURRENT_USER, "Software\\SOTE", 0, NULL, REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &key, &disposition);
    if (result == ERROR_SUCCESS) {
        RegSetValueExA(key, "GraphicsQuality", 0, REG_DWORD, (BYTE*)&config->graphics_quality, sizeof(DWORD));
        RegSetValueExA(key, "SoundEnabled", 0, REG_DWORD, (BYTE*)&config->sound_enabled, sizeof(DWORD));
        RegSetValueExA(key, "FullscreenMode", 0, REG_DWORD, (BYTE*)&config->fullscreen_mode, sizeof(DWORD));
        RegSetValueExA(key, "SavePath", 0, REG_SZ, (BYTE*)config->save_path, strlen(config->save_path) + 1);
        
        RegCloseKey(key);
        return TRUE;
    }
    
    return FALSE;
}

/*
 * Timer functions implementation
 * [RESOURCE:TIMER] timeGetTime
 * PROV: va=0x401000+, iat=0x4c488c, src=imports.WINMM.dll,r2: iij,evidence.curated.json
 * NOTE: Millisecond timer
 * Evidence: WINMM imports (timeGetTime)
 */

DWORD get_game_time(void)
{
    // Use high-resolution timer from WINMM
/* [RESOURCE:TIMER] timeGetTime
 * PROV: va=0x401000+, iat=0x4c488c, src=imports.WINMM.dll,r2: iij,evidence.curated.json
 * NOTE: Millisecond timer
 */
    #ifdef SOTE_DISCLESS
    // Use system time for disc-less mode
    static DWORD start_time = 0;
    if (start_time == 0) {
        start_time = GetTickCount();
    }
    return GetTickCount() - start_time;
    #else
    return timeGetTime();
    #endif
}

void update_game_timer(GameState* state)
{
    if (state) {
        state->game_timer = get_game_time();
    }
}

/*
 * RE-AGENT-TRAILER-JSON
 * {
 *   "artifact_sha256": "be596ee755afbd4f3a50de366a07866d8dfed032f3341b63f539e5f93773ff77",
 *   "agent": "binary-reimplementation-engineer", 
 *   "call_id": "UNKNOWN",
 *   "inputs": ["evidence.curated.json", "layouts.curated.json", "mappings.json"],
 *   "schema_version": "1.0.0"
 * }
 */