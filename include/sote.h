/*
 * RE-AGENT-BANNER
 * Agent: binary-reimplementation-engineer
 * Call ID: UNKNOWN
 * Artifact SHA256: be596ee755afbd4f3a50de366a07866d8dfed032f3341b63f539e5f93773ff77
 * Provenance: evidence.curated.json, layouts.curated.json, mappings.json
 */

#ifndef SOTE_H
#define SOTE_H

#ifdef _WIN32
#include <windows.h>
#include <ddraw.h>
#else
#include "windows_compat.h"  // PROV: build shim only; no runtime semantics assumed
#endif

#include "types.h"
#include "priority_functions.h"
#include "final_31_functions.h"

// Function declarations based on binary analysis
// Evidence: Offset 4279744, Size: 423, Signature: entry0 ();
void entry_point(void);

// Evidence: Offset 4377150, Size: 25, Signature adapted for SDL2 compatibility  
// NOTE: SDL2 redefines main to SDL_main(int argc, char *argv[])
int main(int argc, char** argv);

// Evidence: Offset 4482206, Size: 103, Signature: fcn_0044649e (char **arg_8h, char **arg_ch, int32_t arg_10h, int32_t arg_14h);
int game_main_loop(char** argv, char** envp, int argc, DWORD init_flags);

// Evidence: Offset 4444842, Size: 95, Signature: fcn_0043d2aa (int32_t arg_8h, int32_t arg_ch);
void* memory_allocator(int size, int flags);

// Evidence: Offset 4284632, Size: 31, Signature: fcn_004160d8 (int32_t arg_ch);
void debug_output(int level);

// Evidence: Offset 4446520, Size: 74, Signature: fcn_0043d938 ();
void error_handler(void);

// Evidence: Offset 4461941, Size: 83, Signature: fcn_00441575 ();
void system_info_getter(void);

// Evidence: Offset 4284544, Size: 88, Signature: fcn_00416080 (int32_t arg_4h, int32_t arg_8h, int32_t arg_ch);
int string_utility(int src, int dest, int len);

// Evidence: Offset 4463569, Size: 16, Signature: fcn_00441bd1 ();
void cleanup_handler(void);

// Evidence: Offset 4463563, Size: 6, Signature: fcn_00441bcb ();
void exit_handler(void);

// Global variables (inferred from data section analysis)
extern GameState* g_game_state;
extern GraphicsContext* g_graphics_context;
extern ConfigData* g_config_data;

// Utility macros
#define SOTE_ASSERT(condition) \
    do { \
        if (!(condition)) { \
            error_handler(); \
            exit_handler(); \
        } \
    } while (0)

#define SOTE_LOG(level, message) \
    do { \
        debug_output(level); \
    } while (0)

// DirectX helper functions (inferred from imports)
HRESULT initialize_directdraw(GraphicsContext* ctx);
void shutdown_directdraw(GraphicsContext* ctx);
HRESULT create_surfaces(GraphicsContext* ctx);

// Configuration functions (inferred from registry imports)
BOOL load_config(ConfigData* config);
BOOL save_config(const ConfigData* config);

// Timer functions (inferred from WINMM imports)
DWORD get_game_time(void);
void update_game_timer(GameState* state);

#endif // SOTE_H

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