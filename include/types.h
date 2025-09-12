/*
 * RE-AGENT-BANNER
 * Agent: binary-reimplementation-engineer
 * Call ID: UNKNOWN
 * Artifact SHA256: be596ee755afbd4f3a50de366a07866d8dfed032f3341b63f539e5f93773ff77
 * Provenance: evidence.curated.json, layouts.curated.json, mappings.json
 */

#ifndef SOTE_TYPES_H
#define SOTE_TYPES_H

#ifdef _WIN32
#include <windows.h>
#include <ddraw.h>
#endif
// Note: Windows compatibility types are provided by windows_compat.h
// which is included via sote.h for non-Windows builds

// Forward declarations
typedef struct GameState GameState;
typedef struct GraphicsContext GraphicsContext;
typedef struct ConfigData ConfigData;

// Main game state structure
// Size: ~1024 bytes
// Evidence: Stack frame analysis and DirectX imports
struct GameState {
    GraphicsContext* graphics_context;  // Offset: 0, Size: 4
    DWORD game_timer;                   // Offset: 4, Size: 4
    HWND window_handle;                 // Offset: 8, Size: 4
    BOOL is_active;                     // Offset: 12, Size: 4
    char config_data[256];              // Offset: 16, Size: 256
    // Additional padding/fields inferred from binary analysis
    char reserved[752];                 // Padding to reach estimated 1024 bytes
};

// DirectDraw graphics context
// Size: ~256 bytes
// Evidence: DirectDraw imports and graphics function analysis
struct GraphicsContext {
    IDirectDraw* dd_object;             // Offset: 0, Size: 4
    IDirectDrawSurface* primary_surface; // Offset: 4, Size: 4
    IDirectDrawSurface* back_buffer;    // Offset: 8, Size: 4
    IDirectDrawPalette* palette;        // Offset: 12, Size: 4
    DWORD screen_width;                 // Offset: 16, Size: 4
    DWORD screen_height;                // Offset: 20, Size: 4
    DWORD color_depth;                  // Offset: 24, Size: 4
    char reserved[228];                 // Padding to reach estimated 256 bytes
};

// Application configuration
// Size: ~272 bytes (4 + 4 + 4 + 260)
// Evidence: Registry imports and file I/O patterns
struct ConfigData {
    DWORD graphics_quality;             // Offset: 0, Size: 4
    BOOL sound_enabled;                 // Offset: 4, Size: 4
    BOOL fullscreen_mode;               // Offset: 8, Size: 4
    char save_path[MAX_PATH];           // Offset: 12, Size: 260
};

// Function pointer types
typedef int (*MainLoopFunc)(char** argv, char** envp, int argc, DWORD flags);
typedef void* (*AllocatorFunc)(int size, int flags);
typedef void (*ErrorHandlerFunc)(void);

// Constants derived from binary analysis
#define SOTE_MAX_BUFFER_SIZE 64
#define SOTE_CONFIG_SIZE 256
#define SOTE_GRAPHICS_CONTEXT_SIZE 256
#define SOTE_GAME_STATE_SIZE 1024

#endif // SOTE_TYPES_H

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