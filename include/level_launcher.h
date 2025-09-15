#ifndef LEVEL_LAUNCHER_H
#define LEVEL_LAUNCHER_H

#include <stdint.h>
#include "types.h"

/*
 * SOTE Level Launcher for Testing
 * Allows direct level launch with default save data
 */

// Level definitions based on SAN intro files and data DLLs
typedef enum {
    LEVEL_00_LOGO = 0,      // Logo/Company intro
    LEVEL_01_TUTORIAL = 1,  // First playable level
    LEVEL_02_CITY = 2,      // City level
    LEVEL_03_SEWERS = 3,    // Sewer level
    LEVEL_04_FACTORY = 4,   // Factory + Boss
    LEVEL_05_SHIP = 5,      // Ship + Boss
    LEVEL_06_CANYON = 6,    // Canyon level
    LEVEL_07_CAVES = 7,     // Cave level
    LEVEL_08_BASE = 8,      // Imperial base
    LEVEL_09_DESTROYER = 9, // Star Destroyer + Boss
    LEVEL_10_ENDOR = 10,    // Endor surface
    LEVEL_11_FINALE = 11,   // Final level (win/lose)
    LEVEL_COUNT = 12
} LevelID;

// Level metadata for display and loading
typedef struct {
    LevelID id;
    const char* name;
    const char* description;
    const char* intro_video;    // SAN file for intro
    const char* boss_video;     // SAN file for boss (if any)
    int data_dll_index;         // Which data##.dll contains level data
    BOOL has_boss;
    BOOL is_final;
} LevelInfo;

// Save game structure for level testing
typedef struct {
    char player_name[32];
    int current_level;
    int lives_remaining;
    int score;
    int health;
    int shields;
    int weapon_type;
    int weapon_ammo;
    BOOL has_jetpack;
    BOOL has_thermal_detonators;
    char level_progress[LEVEL_COUNT]; // Progress flags per level
} TestSaveData;

// Level launcher interface
extern const LevelInfo LEVEL_DATABASE[LEVEL_COUNT];

// Initialize level launcher system
int level_launcher_init(GameState* state);

// Launch directly into specified level with default save
int level_launcher_start_level(GameState* state, LevelID level_id);

// Create default save data for testing
TestSaveData* level_launcher_create_default_save(LevelID target_level);

// Load level resources (DLL + assets)
int level_launcher_load_resources(LevelID level_id);

// Cleanup level launcher
void level_launcher_cleanup(void);

// Utility functions
const char* level_get_name(LevelID level_id);
const char* level_get_description(LevelID level_id);
BOOL level_has_boss(LevelID level_id);

#endif /* LEVEL_LAUNCHER_H */