/*
 * SOTE Level Launcher Implementation
 * Allows direct testing of individual levels with default save data
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/level_launcher.h"
#include "../include/runtime_loaders.h"
#include "../include/sote.h"

// Level database with metadata for all 12 levels
const LevelInfo LEVEL_DATABASE[LEVEL_COUNT] = {
    {LEVEL_00_LOGO, "Company Logo", "LucasArts logo and intro sequence", "l00logo.san", NULL, 0, FALSE, FALSE},
    {LEVEL_01_TUTORIAL, "Dash Rendar's Ship", "Tutorial level aboard the Outrider", "l01intro.san", NULL, 1, FALSE, FALSE},
    {LEVEL_02_CITY, "Mos Eisley", "City streets and cantina", "l02intro.san", NULL, 2, FALSE, FALSE},
    {LEVEL_03_SEWERS, "Sewers", "Underground tunnel system", "l03intro.san", NULL, 3, FALSE, FALSE},
    {LEVEL_04_FACTORY, "Factory", "Imperial factory with AT-ST boss", "l04intro.san", "l04boss.san", 4, TRUE, FALSE},
    {LEVEL_05_SHIP, "Imperial Ship", "Boarding action with boss fight", "l05intro.san", "l05boss.san", 5, TRUE, FALSE},
    {LEVEL_06_CANYON, "Canyon", "Swoop bike chase sequence", "l06intro.san", NULL, 6, FALSE, FALSE},
    {LEVEL_07_CAVES, "Crystal Caves", "Underground crystal formations", "l07intro.san", NULL, 7, FALSE, FALSE},
    {LEVEL_08_BASE, "Imperial Base", "Infiltrating the base", "l08intro.san", NULL, 8, FALSE, FALSE},
    {LEVEL_09_DESTROYER, "Star Destroyer", "Aboard the massive ship with IG-88", "l09boss.san", "l09boss.san", 9, TRUE, FALSE},
    {LEVEL_10_ENDOR, "Endor Surface", "Forest moon surface", "l10intro.san", NULL, 10, FALSE, FALSE},
    {LEVEL_11_FINALE, "Final Confrontation", "Final level with multiple endings", "l11win.san", "l11lose.san", 11, TRUE, TRUE}
};

static DLL_ARCHIVE_MANAGER* g_level_manager = NULL;
static TestSaveData* g_current_save = NULL;

// Initialize the level launcher system
int level_launcher_init(GameState* state) {
    if (!state) {
        fprintf(stderr, "[LEVEL] Invalid game state\n");
        return -1;
    }

    // Initialize DLL archive manager for level data
    g_level_manager = dll_archive_init();
    if (!g_level_manager) {
        fprintf(stderr, "[LEVEL] Failed to initialize DLL manager\n");
        return -1;
    }

    fprintf(stderr, "[LEVEL] Level launcher initialized successfully\n");
    return 0;
}

// Create default save data for testing a specific level
TestSaveData* level_launcher_create_default_save(LevelID target_level) {
    if (target_level >= LEVEL_COUNT) {
        fprintf(stderr, "[LEVEL] Invalid level ID: %d\n", target_level);
        return NULL;
    }

    TestSaveData* save = (TestSaveData*)malloc(sizeof(TestSaveData));
    if (!save) {
        fprintf(stderr, "[LEVEL] Failed to allocate save data\n");
        return NULL;
    }

    // Initialize with default testing values
    strcpy(save->player_name, "TestRunner");
    save->current_level = target_level;
    save->lives_remaining = 5;           // Full lives for testing
    save->score = target_level * 10000;  // Progressive score
    save->health = 100;                  // Full health
    save->shields = 100;                 // Full shields
    save->weapon_type = 2;               // Default blaster
    save->weapon_ammo = 999;             // Unlimited ammo for testing
    save->has_jetpack = (target_level >= 3); // Jetpack from level 3+
    save->has_thermal_detonators = (target_level >= 5); // Detonators from level 5+

    // Mark all previous levels as completed
    memset(save->level_progress, 0, LEVEL_COUNT);
    for (int i = 0; i < target_level && i < LEVEL_COUNT; i++) {
        save->level_progress[i] = 1; // Mark as completed
    }

    fprintf(stderr, "[LEVEL] Created default save for %s (Level %d)\n",
            LEVEL_DATABASE[target_level].name, target_level);

    return save;
}

// Load resources for a specific level
int level_launcher_load_resources(LevelID level_id) {
    if (level_id >= LEVEL_COUNT || !g_level_manager) {
        fprintf(stderr, "[LEVEL] Cannot load resources for level %d\n", level_id);
        return -1;
    }

    const LevelInfo* level = &LEVEL_DATABASE[level_id];

    // Load the corresponding data DLL
    int dll_result = dll_archive_load(g_level_manager, level->data_dll_index);
    if (dll_result != 0) {
        fprintf(stderr, "[LEVEL] Failed to load data%02d.dll for %s\n",
                level->data_dll_index, level->name);
        return -1;
    }

    fprintf(stderr, "[LEVEL] Loaded resources for %s (data%02d.dll)\n",
            level->name, level->data_dll_index);

    // TODO: Load intro video if present
    if (level->intro_video) {
        fprintf(stderr, "[LEVEL] Intro video: %s\n", level->intro_video);
    }

    // TODO: Load boss video if present
    if (level->boss_video) {
        fprintf(stderr, "[LEVEL] Boss video: %s\n", level->boss_video);
    }

    return 0;
}

// Launch directly into a specific level
int level_launcher_start_level(GameState* state, LevelID level_id) {
    if (!state || level_id >= LEVEL_COUNT) {
        fprintf(stderr, "[LEVEL] Invalid parameters for level launch\n");
        return -1;
    }

    const LevelInfo* level = &LEVEL_DATABASE[level_id];

    fprintf(stderr, "[LEVEL] ========================================\n");
    fprintf(stderr, "[LEVEL] LAUNCHING: %s\n", level->name);
    fprintf(stderr, "[LEVEL] Description: %s\n", level->description);
    fprintf(stderr, "[LEVEL] Level ID: %d\n", level_id);
    fprintf(stderr, "[LEVEL] Has Boss: %s\n", level->has_boss ? "Yes" : "No");
    fprintf(stderr, "[LEVEL] ========================================\n");

    // Create default save data for this level
    if (g_current_save) {
        free(g_current_save);
    }
    g_current_save = level_launcher_create_default_save(level_id);
    if (!g_current_save) {
        fprintf(stderr, "[LEVEL] Failed to create save data\n");
        return -1;
    }

    // Load level resources
    int load_result = level_launcher_load_resources(level_id);
    if (load_result != 0) {
        fprintf(stderr, "[LEVEL] Failed to load level resources\n");
        return -1;
    }

    // Transition to a new game state for level play
    // TODO: Add GAME_STATE_LEVEL_PLAY to the state machine
    fprintf(stderr, "[LEVEL] Level ready for play!\n");
    fprintf(stderr, "[LEVEL] Player: %s | Lives: %d | Health: %d | Shields: %d\n",
            g_current_save->player_name,
            g_current_save->lives_remaining,
            g_current_save->health,
            g_current_save->shields);

    return 0;
}

// Utility function to get level name
const char* level_get_name(LevelID level_id) {
    if (level_id >= LEVEL_COUNT) {
        return "Unknown Level";
    }
    return LEVEL_DATABASE[level_id].name;
}

// Utility function to get level description
const char* level_get_description(LevelID level_id) {
    if (level_id >= LEVEL_COUNT) {
        return "No description available";
    }
    return LEVEL_DATABASE[level_id].description;
}

// Check if level has a boss
BOOL level_has_boss(LevelID level_id) {
    if (level_id >= LEVEL_COUNT) {
        return FALSE;
    }
    return LEVEL_DATABASE[level_id].has_boss;
}

// Cleanup level launcher resources
void level_launcher_cleanup(void) {
    if (g_level_manager) {
        dll_archive_cleanup(g_level_manager);
        g_level_manager = NULL;
    }

    if (g_current_save) {
        free(g_current_save);
        g_current_save = NULL;
    }

    fprintf(stderr, "[LEVEL] Level launcher cleanup complete\n");
}