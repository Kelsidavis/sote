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

#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include "../include/sote.h"
#include "../include/types.h"

#ifndef _WIN32
// Linux stub types for Windows compatibility
typedef unsigned long DWORD;
typedef unsigned int UINT;
typedef void* HANDLE;
typedef void* LPVOID;
typedef char* LPSTR;
typedef struct {
    DWORD cb;
} STARTUPINFOA;
#define SEM_FAILCRITICALERRORS 0x0001
#define SEM_NOGPFAULTERRORBOX 0x0002
#define GetStartupInfoA(x) ((void)0)
#define GetCommandLineA() ""
#define GetEnvironmentStrings() NULL
#define GetCurrentProcess() ((HANDLE)0)
// PROV: GetProcessHeap removed - NOT in IAT (apis.manifest.json)
// TODO_EVID: GetProcessHeap phantom API removed per verification.report.json
#define HeapCreate(a,b,c) ((HANDLE)0)
#define SetErrorMode(x) ((void)0)
#define SetUnhandledExceptionFilter(x) ((void)0)
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
    
    // Call main function
    // PROV: Call to main @ 0x0042d63e (offset 4377150) from evidence.curated.json
    // PROV: main signature shows int32_t for third param @ functions.key_functions[1]
    result = main(argv, (char**)env_block, argc);
    
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
 * DirectX helper functions implementation
 * Evidence: DirectDraw imports (DirectDrawCreate, DirectDrawEnumerateA)
 */

HRESULT initialize_directdraw(GraphicsContext* ctx)
{
    if (!ctx) {
        return E_INVALIDARG;
    }
    
    /* [RESOURCE:VIDEO] DirectDrawCreate
     * PROV: va=0x401000+, iat=0x4c4cec, src=imports.DDRAW.dll
     * NOTE: Initialize DirectDraw subsystem for hardware-accelerated graphics
     * RESOURCE: Primary display adapter
     */
    HRESULT hr = DirectDrawCreate(NULL, &ctx->dd_object, NULL);
    if (FAILED(hr)) {
        return hr;
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
    if (!ctx || !ctx->dd_object) {
        return E_INVALIDARG;
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
/* [RESOURCE:TIMER] timeGetTime
 * PROV: va=0x401000+, iat=0x4c488c, src=imports.WINMM.dll,r2: iij,evidence.curated.json
 * NOTE: Millisecond timer
 */
 * Evidence: WINMM imports (timeGetTime)
 */

DWORD get_game_time(void)
{
    // Use high-resolution timer from WINMM
/* [RESOURCE:TIMER] timeGetTime
 * PROV: va=0x401000+, iat=0x4c488c, src=imports.WINMM.dll,r2: iij,evidence.curated.json
 * NOTE: Millisecond timer
 */
    return timeGetTime();
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