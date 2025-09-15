/*
 * RE-AGENT-BANNER
 * Agent: binary-reimplementation-engineer
 * Call ID: coord-interactive-title-001
 * Artifact SHA256: df51483219c0d13ce702aaee6df5999c1f9a12e0dfde2f6848890ab963e1627a
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
typedef struct MenuState MenuState;

// Game state enumeration for extended state machine
typedef enum {
    GAME_STATE_SPLASH = 0,      // Initial splash screen
    GAME_STATE_TITLE = 1,       // Interactive title menu
    GAME_STATE_START_SKELETON = 2,  // Start game skeleton
    GAME_STATE_LEVEL_SELECT = 3,    // Level selection for testing
    GAME_STATE_LEVEL_PLAY = 4       // Active level gameplay
} GameStateType;

// Menu entry structure
typedef struct {
    char* text;                 // Menu entry text
    BOOL enabled;               // Whether the entry is selectable
    int action_id;              // Action to perform when selected
} MenuEntry;

// Menu state structure
struct MenuState {
    MenuEntry* entries;         // Array of menu entries
    int entry_count;            // Number of menu entries
    int selected_index;         // Currently selected entry index
    BOOL visible;               // Whether the menu is visible
};

// Main game state structure
// Size: ~1024 bytes
// Evidence: Stack frame analysis and DirectX imports
struct GameState {
    GraphicsContext* graphics_context;  // Offset: 0, Size: 4
    DWORD game_timer;                   // Offset: 4, Size: 4
    HWND window_handle;                 // Offset: 8, Size: 4
    BOOL is_active;                     // Offset: 12, Size: 4
    GameStateType current_state;        // Offset: 16, Size: 4 - State machine current state
    MenuState menu_state;               // Offset: 20, Size: ~16 - Menu state for title screen
    char config_data[256];              // Offset: 36, Size: 256
    // Additional padding/fields inferred from binary analysis
    char reserved[732];                 // Padding to reach estimated 1024 bytes (adjusted for new fields)
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
 *   "artifact_sha256": "df51483219c0d13ce702aaee6df5999c1f9a12e0dfde2f6848890ab963e1627a",
 *   "agent": "binary-reimplementation-engineer", 
 *   "call_id": "coord-interactive-title-001",
 *   "inputs": ["evidence.curated.json", "layouts.curated.json", "mappings.json"],
 *   "schema_version": "1.0.0"
 * }
 */