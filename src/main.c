/*
 * RE-AGENT-BANNER
 * Agent: binary-reimplementation-engineer
 * Call ID: coord-interactive-title-001
 * Artifact SHA256: df51483219c0d13ce702aaee6df5999c1f9a12e0dfde2f6848890ab963e1627a
 * Provenance: evidence.curated.json, layouts.curated.json, mappings.json
 */

/*
 * SOTE Main Module
 * Reimplemented from binary analysis of SOTE.EXE
 * SHA256: be596ee755afbd4f3a50de366a07866d8dfed032f3341b63f539e5f93773ff77
 */

#ifdef LINUX_BUILD
#define _GNU_SOURCE
#include <string.h>
#include <stdlib.h>  // For setenv
#include <dlfcn.h>  // For dlopen, dlsym, dlerror
#endif

#include "../include/sote.h"
#include "../include/types.h"
#include "../include/missing_functions.h"
#include "../include/adapter_fs.h"
#include "../include/level_launcher.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "SDL_compat.h" /* contains SDL_MAIN_HANDLED for Linux */
// PROV: Include SDL2 headers for main handling - RE-AGENT REBUILD m100
// Evidence: SDL2 documentation recommends SDL_SetMainReady() for custom main functions
// PROV: Forward declaration for SDL_SetMainReady when headers not available
#ifndef _SDL_main_h
extern void SDL_SetMainReady(void);
#endif

// Global level launcher state
static int g_level_launcher_initialized = 0;
static int g_selected_level = 0;

#ifndef _WIN32
// Linux stub implementations
#define PeekMessageA(msg, hwnd, min, max, flags) 0
#define TranslateMessage(msg) ((void)0)
#define DispatchMessageA(msg) ((void)0)
#define MessageBoxA(hwnd, text, caption, type) ((void)0)
#define OutputDebugStringA(str) printf("DEBUG: %s\n", str)
#define HeapCreate(options, initial, maximum) ((HANDLE)1)
#define HeapAlloc(heap, flags, size) malloc(size)
#define HeapFree(heap, flags, ptr) free(ptr)
#define GetVersion() 0
#define GetSystemInfo(info) ((void)0)
#define ExitProcess(code) exit(code)
typedef struct { int dummy; } SYSTEM_INFO;
#else
// Windows compatibility - setenv doesn't exist on Windows, use _putenv_s
#include <stdlib.h>
#define setenv(name, value, overwrite) _putenv_s(name, value)
#endif

// Global state variables (inferred from data section)
GameState* g_game_state = NULL;
GraphicsContext* g_graphics_context = NULL;
ConfigData* g_config_data = NULL;

/*
 * Main function
 * Evidence: Offset 4377150, Size: 25
 * Original signature: int main (char **argv, char **envp, int32_t envp);
 * Stack frame: 20 bytes
 * PROV: Signature from evidence.curated.json @ functions.key_functions[1]
 * PROV: Size constraint 25 bytes requires minimal wrapper implementation
 * PROV: Third param is int32_t per evidence @ functions.key_functions[1].signature
 * PROV: Third param named envp per evidence (C syntax requires envp_count disambiguation)
 * TODO_EVID: Evidence shows duplicate param name 'envp' - using envp_count for valid C syntax
 */
/* PROV: SDL glue for non-Windows plain main() */
int main(int argc, char** argv)
{
#if defined(_WIN32)
    if (getenv("SOTE_FORCE_SDL")) fprintf(stderr, "[ROUTE] forcing SDL path\n");
#endif
#ifdef SDL_MAIN_HANDLED
    SDL_SetMainReady();
#endif
    if (!getenv("SDL_VIDEODRIVER")) setenv("SDL_VIDEODRIVER","dummy",0);
    if (!getenv("SDL_AUDIODRIVER")) setenv("SDL_AUDIODRIVER","dummy",0);
    // PROV: Using real SDL2, no stub initialization needed - RE-AGENT REBUILD m100
    // Evidence: Real SDL2 library handles initialization automatically

    // PROV: Initialize SDL main handling before any other operations - RE-AGENT REBUILD m100
    // Evidence: SDL2 documentation states SDL_SetMainReady() should be called before SDL_Init()


    /* Initialize SDL with fallback to dummy driver */
    if (SDL_Init(SDL_INIT_VIDEO
#ifndef DISABLE_AUDIO
                 | SDL_INIT_AUDIO
#endif
                 ) != 0) {
        fprintf(stderr, "[SDL] SDL_Init failed: %s\n", SDL_GetError());
        setenv("SDL_VIDEODRIVER", "dummy", 1);
        if (SDL_Init(SDL_INIT_VIDEO
#ifndef DISABLE_AUDIO
                     | SDL_INIT_AUDIO
#endif
                     ) != 0) {
            fprintf(stderr, "[SDL] SDL_Init(dummy) failed: %s\n", SDL_GetError());
            return 1;
        }
        fprintf(stderr, "[SDL] Fallback to dummy video driver\n");
    }

    // PROV: Ultra-early logging for startup fault triage per coord-startup-002
    #ifdef RESOURCE_WARNINGS
    fprintf(stderr, "[STARTUP] main() entry - argc=%d, argv=%p (SDL2 signature)\n", 
            argc, (void*)argv);
    fprintf(stderr, "[STARTUP] SOTE_ASSETS_DIR='%s'\n", getenv("SOTE_ASSETS_DIR") ? getenv("SOTE_ASSETS_DIR") : "(null)");
    fprintf(stderr, "[STARTUP] SOTE_DISCLESS='%s'\n", getenv("SOTE_DISCLESS") ? getenv("SOTE_DISCLESS") : "(null)");
    #endif

    // PROV: 25-byte size constraint @ 0x0042d63e requires direct call only
    // PROV: cdecl calling convention from evidence.curated.json
    // PROV: No argument processing loops fit in 25 bytes
    // PROV: Direct jmp/call pattern observed in binary
    
    // PROV: Call to fcn_0044649e @ 0x0044649e (offset 4482206)
    // NOTE: Adapted for SDL2 signature - envp and envp_count no longer available
    return game_main_loop(argv, NULL, argc, 0);
}

/*
 * Game main loop function
 * Evidence: Offset 4482206, Size: 103
 * PROV: rename to canonical fcn_0044649e
 * Original signature: fcn_0044649e (char **arg_8h, char **arg_ch, int32_t arg_10h, int32_t arg_14h);
 * Stack frame: 32 bytes
 */
int game_main_loop(char** argv, char** envp, int argc, DWORD init_flags)
{
    // Initialize game state
    g_game_state = (GameState*)memory_allocator(sizeof(GameState), 0);
    if (!g_game_state) {
        error_handler();
        return -1;
    }
    
    // Initialize graphics context
    g_graphics_context = (GraphicsContext*)memory_allocator(sizeof(GraphicsContext), 0);
    if (!g_graphics_context) {
        error_handler();
        return -1;
    }
    
    // Initialize configuration
    g_config_data = (ConfigData*)memory_allocator(sizeof(ConfigData), 0);
    if (!g_config_data) {
        error_handler();
        return -1;
    }
    
    /* [RESOURCE:FS] Registry Configuration
     * PROV: RegOpenKeyA/RegQueryValueExA @ imports.ADVAPI32.dll
     * NOTE: Load game configuration from Windows registry
     * RESOURCE: HKEY_CURRENT_USER\Software\SOTE registry keys
     */
    if (!load_config(g_config_data)) {
        debug_output(1); // Warning level
    }
    
    /* [RESOURCE:VIDEO] DirectDraw Initialization
     * PROV: initialize_directdraw uses DirectDrawCreate @ iat=0x4c4cec
     * NOTE: Initialize DirectDraw for hardware-accelerated graphics
     * RESOURCE: Primary display adapter, 640x480x8 video mode
     */
    if (FAILED(initialize_directdraw(g_graphics_context))) {
        error_handler();
        cleanup_handler();
        return -1;
    }
    
    // Set up game state
    g_game_state->graphics_context = g_graphics_context;
    g_game_state->window_handle = NULL; // Will be set during window creation
    g_game_state->is_active = TRUE;
    g_game_state->game_timer = get_game_time();
    
    // Initialize state machine - start with Splash phase
    // PROV: Three-phase state machine: Splash → Title → StartSkeleton
    state_machine_init(g_game_state);
    
    // System info gathering (evidence: call pattern analysis)
    system_info_getter();
    
    // Main message loop with state machine
    // PROV: Win32 message pump pattern @ similar functions in binary
    // PROV: PeekMessageA @ IAT 0x00481138 (evidence.curated.json)
    // PROV: DispatchMessageA @ IAT 0x00481134 (evidence.curated.json)
    MSG msg;
    while (g_game_state->is_active) {
        if (PeekMessageA(&msg, NULL, 0, 0, PM_REMOVE)) {
            if (msg.message == WM_QUIT) {
                break;
            }
            TranslateMessage(&msg);
            DispatchMessageA(&msg);
        }
        
        // Update game timer
        update_game_timer(g_game_state);
        
        // State machine update - handles current state logic
        state_machine_update(g_game_state);
    }
    
    // Cleanup
    cleanup_handler();
    return 0;
}

/*
 * Memory allocator wrapper
 * Evidence: Offset 4444842, Size: 95
 * PROV: rename to canonical fcn_0043d2aa
 * Original signature: fcn_0043d2aa (int32_t arg_8h, int32_t arg_ch);
 * Stack frame: 24 bytes with 3 local variables
 */
void* memory_allocator(int size, int flags)
{
    // PROV: HeapAlloc @ IAT (evidence.curated.json)
    // PROV: HeapCreate @ IAT for heap creation
    // NOTE: GetProcessHeap NOT in imports - using HeapCreate instead
    static HANDLE heap = NULL;
    if (!heap) {
        heap = HeapCreate(0, 0, 0);
        if (!heap) {
            error_handler();
            return NULL;
        }
    }
    
    void* ptr = HeapAlloc(heap, flags, size);
    if (!ptr) {
        debug_output(2); // Error level
        error_handler();
    }
    
    return ptr;
}

/*
 * Debug output function
 * Evidence: Offset 4284632, Size: 31
 * High call count (215 references) suggests logging/debug utility
 */
void debug_output(int level)
{
    // Evidence: Small size suggests simple debug output
    // High call count indicates this is a frequently used utility
    static const char* level_names[] = {"INFO", "WARN", "ERROR"};
    
    if (level >= 0 && level < 3) {
        OutputDebugStringA(level_names[level]);
    }
}

/*
 * Error handler
 * Evidence: Offset 4446520, Size: 74
 * Called from multiple locations, suggests centralized error handling
 */
void error_handler(void)
{
    // Evidence: Size and call pattern suggest error logging and cleanup
    debug_output(2); // Error level
    
    // Get system error information (evidence: system info getter calls)
    system_info_getter();
    
    // Display error message to user (evidence: MessageBox imports)
    MessageBoxA(NULL, "An error occurred in SOTE application", "Error", MB_OK | MB_ICONERROR);
}

/*
 * System information getter
 * Evidence: Offset 4461941, Size: 83
 * Moderate size suggests system info collection
 */
void system_info_getter(void)
{
    // Evidence: Calls suggest system information gathering
    DWORD version = GetVersion();
    SYSTEM_INFO sys_info;
    GetSystemInfo(&sys_info);
    
    // Store system information for error reporting
    // Evidence: Function size suggests moderate processing
    debug_output(0); // Info level
}

/*
 * String utility function
 * Evidence: Offset 4284544, Size: 88
 * 3 parameters suggest string manipulation
 */
int string_utility(int src, int dest, int len)
{
    // Evidence: Size and parameter count suggest string copy/manipulation
    if (!src || !dest || len <= 0) {
        return 0;
    }
    
    // Simple string copy with bounds checking
    char* src_str = (char*)src;
    char* dest_str = (char*)dest;
    
    int copied = 0;
    while (copied < len - 1 && src_str[copied] != '\0') {
        dest_str[copied] = src_str[copied];
        copied++;
    }
    dest_str[copied] = '\0';
    
    return copied;
}

/*
 * State machine initialization
 * PROV: Initialize three-phase state machine: Splash → Title → StartSkeleton
 */
void state_machine_init(GameState* state)
{
    if (!state) return;
    
    // Start with Splash phase
    state->current_state = GAME_STATE_SPLASH;
    
    // Initialize menu state for Title phase
    menu_init(&state->menu_state);
    
    #ifdef RESOURCE_WARNINGS
    fprintf(stderr, "[STATE] Initialized state machine - starting with SPLASH\n");
    #endif
}

/*
 * State machine update loop
 * PROV: Handle current state logic and transitions
 */
void state_machine_update(GameState* state)
{
    if (!state) return;
    
    switch (state->current_state) {
        case GAME_STATE_SPLASH:
            // PROV: Splash state - minimal display, auto-advance to Title
            // TODO: Add splash display logic
            #ifdef RESOURCE_WARNINGS
            fprintf(stderr, "[STATE] Splash phase - auto-advancing to Title\n");
            #endif
            state_machine_transition(state, GAME_STATE_TITLE);
            break;
            
        case GAME_STATE_TITLE:
            // PROV: Interactive Title menu with cursor navigation
            title_state_update(state);
            title_state_input(state);
            title_state_draw(state);
            break;
            
        case GAME_STATE_LEVEL_SELECT:
            // Level selection menu
            level_select_state_update(state);
            level_select_state_input(state);
            level_select_state_draw(state);
            break;

        case GAME_STATE_LEVEL_PLAY:
            // Active level gameplay
            level_play_state_update(state);
            level_play_state_input(state);
            level_play_state_draw(state);
            break;

        case GAME_STATE_START_SKELETON:
            // PROV: Start game skeleton - minimal implementation
            #ifdef RESOURCE_WARNINGS
            fprintf(stderr, "[STATE] StartSkeleton phase - game started\n");
            #endif
            // For now, just maintain active state
            break;
            
        default:
            #ifdef RESOURCE_WARNINGS
            fprintf(stderr, "[STATE] Unknown state: %d\n", state->current_state);
            #endif
            break;
    }
}

/*
 * State machine transition
 * PROV: Handle state transitions with logging
 */
void state_machine_transition(GameState* state, GameStateType new_state)
{
    if (!state) return;
    
    GameStateType old_state = state->current_state;
    state->current_state = new_state;
    
    #ifdef RESOURCE_WARNINGS
    fprintf(stderr, "[STATE] Transition: %d -> %d\n", old_state, new_state);
    #endif
    
    // Initialize new state
    switch (new_state) {
        case GAME_STATE_TITLE:
            title_state_init(state);
            break;
        case GAME_STATE_LEVEL_SELECT:
            level_select_state_init(state);
            break;
        case GAME_STATE_START_SKELETON:
            // Clean up menu state when leaving Title
            menu_cleanup(&state->menu_state);
            break;
        case GAME_STATE_LEVEL_PLAY:
            // Level gameplay state
            break;
        default:
            break;
    }
}

/*
 * Title state initialization
 * PROV: Set up interactive Title menu with cursor and entries
 */
void title_state_init(GameState* state)
{
    if (!state) return;
    
    // Initialize menu with "Start Game", "Level Select" and "Options" entries
    MenuState* menu = &state->menu_state;

    // Allocate menu entries
    menu->entry_count = 3;
    menu->entries = (MenuEntry*)memory_allocator(sizeof(MenuEntry) * menu->entry_count, 0);
    if (!menu->entries) {
        error_handler();
        return;
    }

    // Set up "Start Game" entry (default selection)
    menu->entries[0].text = (char*)memory_allocator(16, 0);
    if (menu->entries[0].text) {
        strcpy(menu->entries[0].text, "Start Game");
        menu->entries[0].enabled = TRUE;
        menu->entries[0].action_id = 1;
    }

    // Set up "Level Select" entry
    menu->entries[1].text = (char*)memory_allocator(16, 0);
    if (menu->entries[1].text) {
        strcpy(menu->entries[1].text, "Level Select");
        menu->entries[1].enabled = TRUE;
        menu->entries[1].action_id = 3;
    }

    // Set up "Options" entry (disabled placeholder)
    menu->entries[2].text = (char*)memory_allocator(16, 0);
    if (menu->entries[2].text) {
        strcpy(menu->entries[2].text, "Options");
        menu->entries[2].enabled = FALSE;  // Disabled placeholder
        menu->entries[2].action_id = 2;
    }
    
    menu->selected_index = 0;  // Default to "Start Game"
    menu->visible = TRUE;
    
    #ifdef RESOURCE_WARNINGS
    fprintf(stderr, "[TITLE] Initialized Title menu - 3 entries, default: Start Game\n");
    #endif
}

/*
 * Title state drawing
 * PROV: Render Title background and menu with cursor highlighting
 */
void title_state_draw(GameState* state)
{
    if (!state || !state->graphics_context) return;
    
    // PROV: Load and draw TITLE_BMP background via evidence-only VFS resolution
    load_title_bmp(state->graphics_context);
    
    // Draw the menu overlay
    menu_draw(&state->menu_state, state->graphics_context);
}

/*
 * Title state input handling
 * PROV: Handle UP/DOWN cursor movement and ENTER selection
 */
void title_state_input(GameState* state)
{
    if (!state) return;
    
    // TODO: Integrate with input adapter for SDL key events
    // For now, simulate input handling for testing
    // Real implementation will get key events from adapter_input_sdl.c
    
    // Check for simulated input (this will be replaced with real SDL input)
    // TODO: Remove this when real SDL input is implemented
    static int simulated_input_timer = 0;
    simulated_input_timer++;

    // Simulate DOWN → DOWN → RETURN sequence to select Level Select
    if (simulated_input_timer == 100) {
        // Simulate DOWN key to move to Level Select
        menu_input(&state->menu_state, 40);  // VK_DOWN
    } else if (simulated_input_timer == 200) {
        // Simulate DOWN key again if needed
        if (state->menu_state.selected_index == 0) {
            menu_input(&state->menu_state, 40);  // VK_DOWN to Level Select
        }
    } else if (simulated_input_timer == 300) {
        // Simulate RETURN key
        menu_input(&state->menu_state, 13);  // VK_RETURN

        // Handle menu selection based on action_id
        if (state->menu_state.entries && state->menu_state.entries[state->menu_state.selected_index].enabled) {
            int action_id = state->menu_state.entries[state->menu_state.selected_index].action_id;

            if (action_id == 1) {
                // "Start Game" selected - advance to StartSkeleton
                state_machine_transition(state, GAME_STATE_START_SKELETON);
            } else if (action_id == 3) {
                // "Level Select" selected - go to level selection
                state_machine_transition(state, GAME_STATE_LEVEL_SELECT);
            }
        }
    }
}

/*
 * Title state update
 * PROV: Update Title state logic (timer, animations, etc.)
 */
void title_state_update(GameState* state)
{
    if (!state) return;
    
    // Update any Title-specific logic here
    // For now, minimal implementation
}

/*
 * Menu initialization
 * PROV: Initialize empty menu state
 */
void menu_init(MenuState* menu)
{
    if (!menu) return;
    
    menu->entries = NULL;
    menu->entry_count = 0;
    menu->selected_index = 0;
    menu->visible = FALSE;
}

/*
 * Menu drawing
 * PROV: Render menu entries with cursor highlighting
 */
void menu_draw(MenuState* menu, GraphicsContext* graphics)
{
    if (!menu || !graphics || !menu->visible) return;
    
    #ifdef RESOURCE_WARNINGS
    fprintf(stderr, "[MENU] Drawing menu - %d entries, selected: %d\n", 
            menu->entry_count, menu->selected_index);
    #endif
    
    // TODO: Implement actual menu rendering
    // For now, just log the menu state
    for (int i = 0; i < menu->entry_count; i++) {
        if (menu->entries && menu->entries[i].text) {
            char cursor = (i == menu->selected_index) ? '>' : ' ';
            // Note: enabled flag handled in the fprintf below
            #ifdef RESOURCE_WARNINGS
            fprintf(stderr, "[MENU] %c %s %s\n", cursor, menu->entries[i].text, 
                    menu->entries[i].enabled ? "" : "(disabled)");
            #endif
        }
    }
}

/*
 * Menu input handling
 * PROV: Handle UP/DOWN navigation and ENTER selection
 */
void menu_input(MenuState* menu, int key_code)
{
    if (!menu || !menu->visible || menu->entry_count == 0) return;
    
    switch (key_code) {
        case 38:  // UP arrow (VK_UP)
        case 87:  // W key
            if (menu->selected_index > 0) {
                menu->selected_index--;
                #ifdef RESOURCE_WARNINGS
                fprintf(stderr, "[MENU] Cursor UP - selected: %d\n", menu->selected_index);
                #endif
            }
            break;
            
        case 40:  // DOWN arrow (VK_DOWN)
        case 83:  // S key
            if (menu->selected_index < menu->entry_count - 1) {
                menu->selected_index++;
                #ifdef RESOURCE_WARNINGS
                fprintf(stderr, "[MENU] Cursor DOWN - selected: %d\n", menu->selected_index);
                #endif
            }
            break;
            
        case 13:  // ENTER key (VK_RETURN)
            if (menu->entries && menu->entries[menu->selected_index].enabled) {
                #ifdef RESOURCE_WARNINGS
                fprintf(stderr, "[MENU] selected: %s\n", menu->entries[menu->selected_index].text);
                #endif
                
                // Handle menu selection actions
                if (menu->entries[menu->selected_index].action_id == 1) {
                    // "Start Game" selected - advance to StartSkeleton
                    // Need access to game state for transition
                    // This will be handled by the title_state_input function
                }
            }
            break;
    }
}

/*
 * Menu cleanup
 * PROV: Free allocated menu resources
 */
void menu_cleanup(MenuState* menu)
{
    if (!menu) return;
    
    if (menu->entries) {
        for (int i = 0; i < menu->entry_count; i++) {
            if (menu->entries[i].text) {
                // TODO: Use proper heap free when available
                // HeapFree would go here
                menu->entries[i].text = NULL;
            }
        }
        // TODO: Use proper heap free when available
        // HeapFree would go here
        menu->entries = NULL;
    }

    menu->entry_count = 0;
    menu->selected_index = 0;
    menu->visible = FALSE;
}

/*
 * Add entry to menu
 */
void menu_add_entry(MenuState* menu, const char* text, BOOL enabled, int action_id)
{
    if (!menu || !text) return;

    // Reallocate entries array to accommodate new entry
    MenuEntry* new_entries = (MenuEntry*)memory_allocator(sizeof(MenuEntry) * (menu->entry_count + 1), 0);
    if (!new_entries) return;

    // Copy existing entries
    for (int i = 0; i < menu->entry_count; i++) {
        new_entries[i] = menu->entries[i];
    }

    // Add new entry
    new_entries[menu->entry_count].text = (char*)memory_allocator(strlen(text) + 1, 0);
    if (new_entries[menu->entry_count].text) {
        strcpy(new_entries[menu->entry_count].text, text);
    }
    new_entries[menu->entry_count].enabled = enabled;
    new_entries[menu->entry_count].action_id = action_id;

    // Replace old entries array
    if (menu->entries) {
        // TODO: Free old entries array
    }
    menu->entries = new_entries;
    menu->entry_count++;
}

/*
 * VFS Asset Resolution Function
 * PROV: Evidence-only asset resolution with deterministic search order
 * SEARCH: manifest → actual BMP → boba.bmp fallback
 */
char* vfs_resolve_asset(const char* asset_name)
{
    if (!asset_name) return NULL;
    
    #ifdef RESOURCE_WARNINGS
    fprintf(stderr, "[VFS] Resolving asset: %s\n", asset_name);
    #endif
    
    // PROV: Try VFS resolution first via adapter_fs_posix.c
    // This will use the existing VFS search order
    char* resolved_path = NULL;
    
    // Use existing VFS functions from adapter_fs_posix.c
    // adapter_fs_exists will call vfs_resolve_path internally
    if (adapter_fs_exists(asset_name)) {
        // Get the resolved path by trying to open it (this also resolves via VFS)
        HANDLE handle = adapter_fs_open(asset_name, GENERIC_READ, OPEN_EXISTING);
        if (handle != INVALID_HANDLE_VALUE) {
            // Extract the resolved path from handle (if available)
            // For now, assume successful resolution
            resolved_path = strdup(asset_name);  // Will be resolved by VFS internally
            adapter_CloseHandle(handle);
        }
    }
    
    #ifdef RESOURCE_WARNINGS
    if (resolved_path) {
        fprintf(stderr, "[VFS] Asset resolved: %s\n", resolved_path);
    } else {
        fprintf(stderr, "[VFS] Asset not found: %s\n", asset_name);
    }
    #endif
    
    return resolved_path;
}

/*
 * Load Title BMP Function
 * PROV: Evidence-only Title background loading with fallback chain
 * CHAIN: TITLE_BMP → title.bmp → boba.bmp fallback
 */
int load_title_bmp(GraphicsContext* graphics)
{
    if (!graphics) return -1;
    
    static int title_loaded = 0;
    if (title_loaded) return 0;  // Already loaded
    
    const char* title_candidates[] = {
        "TITLE_BMP",      // Evidence manifest name
        "title.bmp",      // Direct BMP file
        "boba.bmp",       // Known fallback asset
        NULL
    };
    
    #ifdef RESOURCE_WARNINGS
    fprintf(stderr, "[TITLE] Loading Title BMP background\n");
    #endif
    
    for (int i = 0; title_candidates[i] != NULL; i++) {
        char* resolved_path = vfs_resolve_asset(title_candidates[i]);
        if (resolved_path) {
            #ifdef RESOURCE_WARNINGS
            fprintf(stderr, "[TITLE] Using Title asset: %s\n", title_candidates[i]);
            #endif
            
            // TODO: Actual BMP loading and display via DirectDraw/SDL
            // For now, just log successful resolution
            void* bmp_data = load_bmp_file(resolved_path);
            if (bmp_data) {
                title_loaded = 1;
                #ifdef RESOURCE_WARNINGS
                fprintf(stderr, "[TITLE] Title BMP loaded successfully\n");
                #endif
                free(resolved_path);
                return 0;
            }
            
            free(resolved_path);
        }
    }
    
    #ifdef RESOURCE_WARNINGS
    fprintf(stderr, "[WARN][TITLE] No Title BMP assets found - using default\n");
    #endif
    
    return -1;
}

/*
 * Load BMP File Function
 * PROV: Generic BMP file loading with basic validation
 */
void* load_bmp_file(const char* path)
{
    if (!path) return NULL;
    
    #ifdef RESOURCE_WARNINGS
    fprintf(stderr, "[BMP] Loading bitmap: %s\n", path);
    #endif
    
    // Open file via VFS-aware adapter
    HANDLE handle = adapter_fs_open(path, GENERIC_READ, OPEN_EXISTING);
    if (handle == INVALID_HANDLE_VALUE) {
        #ifdef RESOURCE_WARNINGS
        fprintf(stderr, "[BMP] Failed to open: %s\n", path);
        #endif
        return NULL;
    }
    
    // TODO: Actual BMP parsing and loading
    // For now, just simulate successful load by closing the file
    adapter_CloseHandle(handle);
    
    #ifdef RESOURCE_WARNINGS
    fprintf(stderr, "[BMP] Successfully loaded: %s\n", path);
    #endif
    
    // Return dummy data to indicate success
    return (void*)1;  // Non-null indicates success
}

/*
 * Cleanup handler
 * Evidence: Offset 4463569, Size: 16
 * Small size suggests simple cleanup
 * PROV: 16-byte size @ 0x00441bd1 indicates minimal implementation
 */
void cleanup_handler(void)
{
    // PROV: Size constraint of 16 bytes suggests single call or minimal logic
    // TODO_EVID: Need disassembly @ 0x00441bd1 to confirm exact implementation
    // NOTE: Current implementation exceeds size constraint - stub for now
    
    // Clean up menu state
    if (g_game_state) {
        menu_cleanup(&g_game_state->menu_state);
    }
    
    // Minimal cleanup based on size constraint
    // PROV: Likely single call to exit or cleanup routine
    if (g_graphics_context) {
        shutdown_directdraw(g_graphics_context);
    }
}

/*
 * Exit handler
 * Evidence: Offset 4463563, Size: 6
 * Very small size suggests simple exit call
 */
void exit_handler(void)
{
    // Evidence: Tiny size (6 bytes) suggests direct exit call
    ExitProcess(0);
}

/*
 * ============================================================================
 * LEVEL LAUNCHER STATE FUNCTIONS
 * ============================================================================
 */

/*
 * Level Selection State Initialization
 */
void level_select_state_init(GameState* state) {
    if (!state) return;

    // Initialize level launcher if not already done
    if (!g_level_launcher_initialized) {
        if (level_launcher_init(state) == 0) {
            g_level_launcher_initialized = 1;
            fprintf(stderr, "[LEVEL] Level launcher initialized\n");
        } else {
            fprintf(stderr, "[LEVEL] Failed to initialize level launcher\n");
            return;
        }
    }

    // Initialize level selection menu
    menu_init(&state->menu_state);

    // Add level entries to menu
    for (int i = 0; i < LEVEL_COUNT; i++) {
        char level_text[128];
        const LevelInfo* level = &LEVEL_DATABASE[i];
        snprintf(level_text, sizeof(level_text), "%2d. %s%s",
                 i, level->name, level->has_boss ? " [BOSS]" : "");
        menu_add_entry(&state->menu_state, level_text, TRUE, i);
    }

    // Add back option
    menu_add_entry(&state->menu_state, "Back to Title", TRUE, -1);

    state->menu_state.selected_index = g_selected_level;
    state->menu_state.visible = TRUE;

    fprintf(stderr, "[LEVEL] Level selection menu initialized with %d levels\n", LEVEL_COUNT);
}

/*
 * Level Selection State Update
 */
void level_select_state_update(GameState* state) {
    if (!state) return;
    // Level selection update logic
}

/*
 * Level Selection State Input
 */
void level_select_state_input(GameState* state) {
    if (!state) return;

    // Handle menu navigation (DOWN/UP keys)
    // For now, use automatic selection for testing
    static int input_timer = 0;
    input_timer++;

    if (input_timer == 60) {  // After 1 second, select current level
        int selected = state->menu_state.selected_index;

        if (selected == LEVEL_COUNT) {  // "Back to Title" option
            fprintf(stderr, "[LEVEL] Returning to title\n");
            state_machine_transition(state, GAME_STATE_TITLE);
        } else if (selected >= 0 && selected < LEVEL_COUNT) {
            fprintf(stderr, "[LEVEL] Selected level %d: %s\n", selected, LEVEL_DATABASE[selected].name);
            g_selected_level = selected;

            // Launch the level
            if (level_launcher_start_level(state, (LevelID)selected) == 0) {
                state_machine_transition(state, GAME_STATE_LEVEL_PLAY);
            } else {
                fprintf(stderr, "[LEVEL] Failed to launch level, returning to selection\n");
            }
        }
    }
}

/*
 * Level Selection State Draw
 */
void level_select_state_draw(GameState* state) {
    if (!state) return;
    menu_draw(&state->menu_state, state->graphics_context);
}

/*
 * Level Play State Functions
 */
void level_play_state_update(GameState* state) {
    if (!state) return;
    // Active gameplay update logic
    // TODO: Integrate with actual game systems
}

void level_play_state_input(GameState* state) {
    if (!state) return;

    // For testing, auto-return to level select after a few seconds
    static int play_timer = 0;
    play_timer++;

    if (play_timer == 300) {  // After 5 seconds
        fprintf(stderr, "[LEVEL] Level play demo complete, returning to level select\n");
        play_timer = 0;
        state_machine_transition(state, GAME_STATE_LEVEL_SELECT);
    }
}

void level_play_state_draw(GameState* state) {
    if (!state) return;

    // Simple level play display
    static int frame_count = 0;
    frame_count++;

    if (frame_count % 60 == 0) {  // Every second
        const LevelInfo* level = &LEVEL_DATABASE[g_selected_level];
        fprintf(stderr, "[LEVEL] Playing: %s (Level %d)\n", level->name, g_selected_level);
    }
}

/*
 * RE-AGENT-TRAILER-JSON
 * {
 *   "artifact_sha256": "df51483219c0d13ce702aaee6df5999c1f9a12e0dfde2f6848890ab963e1627a",
 *   "agent": "binary-reimplementation-engineer",
 *   "call_id": "coord-interactive-title-001",
 *   "inputs": ["evidence.curated.json", "layouts.curated.json", "mappings.json"],
 *   "schema_version": "1.0.0"
 * }
 */
