# SOTE Level Launcher

A testing framework for launching individual game levels with default save data.

## Overview

The SOTE Level Launcher allows developers and testers to jump directly to any of the 12 game levels without playing through the entire game. Each level launches with appropriate save data including proper progression, equipment, and stats.

## Identified Levels

Based on analysis of SAN intro files and data DLL structure:

| ID | Level Name           | Description                           | Data File  | Intro Video    | Boss Video   | Features |
|----|---------------------|---------------------------------------|------------|----------------|--------------|----------|
| 0  | Company Logo        | LucasArts logo and intro sequence    | data00.dll| l00logo.san    | -            | -        |
| 1  | Dash Rendar's Ship  | Tutorial level aboard the Outrider   | data01.dll| l01intro.san   | -            | Tutorial |
| 2  | Mos Eisley          | City streets and cantina              | data02.dll| l02intro.san   | -            | -        |
| 3  | Sewers              | Underground tunnel system             | data03.dll| l03intro.san   | -            | Jetpack+ |
| 4  | Factory             | Imperial factory with AT-ST boss     | data04.dll| l04intro.san   | l04boss.san  | Boss     |
| 5  | Imperial Ship       | Boarding action with boss fight       | data05.dll| l05intro.san   | l05boss.san  | Boss + Detonators+ |
| 6  | Canyon              | Swoop bike chase sequence             | data06.dll| l06intro.san   | -            | Vehicle  |
| 7  | Crystal Caves       | Underground crystal formations        | data07.dll| l07intro.san   | -            | -        |
| 8  | Imperial Base       | Infiltrating the base                 | data08.dll| l08intro.san   | -            | -        |
| 9  | Star Destroyer      | Aboard the massive ship with IG-88   | data09.dll| l09boss.san    | l09boss.san  | Boss     |
| 10 | Endor Surface       | Forest moon surface                   | data10.dll| l10intro.san   | -            | -        |
| 11 | Final Confrontation | Final level with multiple endings    | data11.dll| l11win.san     | l11lose.san  | Boss + Final |

## Usage

### Command Line Interface

#### Using the shell script (recommended):
```bash
# List all levels
./test_levels.sh list

# Launch specific level by ID
./test_levels.sh level 4          # Factory level with boss

# Find level by name (partial match)
./test_levels.sh find factory     # Case-insensitive search

# Quick test of key levels
./test_levels.sh quick            # Tests levels 1, 4, 9, 11

# Help
./test_levels.sh help
```

#### Using the launcher directly:
```bash
# Build the launcher
make test-launcher

# Run with Wine (Linux) or directly (Windows)
wine ./build_win/bin/test_level_launcher.exe --list
wine ./build_win/bin/test_level_launcher.exe --level 4
wine ./build_win/bin/test_level_launcher.exe --level-name "Star Destroyer"
```

### Default Save Data

Each level launches with appropriate save data:

- **Player Name**: "TestRunner"
- **Lives**: 5 (full lives for testing)
- **Health**: 100 (full health)
- **Shields**: 100 (full shields)
- **Score**: Progressive (level_id × 10,000)
- **Weapon**: Default blaster with unlimited ammo (999)
- **Equipment**:
  - Jetpack: Available from level 3+
  - Thermal Detonators: Available from level 5+
- **Progress**: All previous levels marked as completed

## Architecture

### Key Components

1. **Level Database** (`LEVEL_DATABASE[]`): Static metadata for all 12 levels
2. **Save System** (`TestSaveData`): Default save game structure
3. **Resource Loader**: Maps levels to data##.dll files
4. **State Machine Integration**: Extends game states for level selection

### New Game States

Extended the original 3-state machine:
- `GAME_STATE_SPLASH` - Splash screen
- `GAME_STATE_TITLE` - Title menu
- `GAME_STATE_START_SKELETON` - Game start
- `GAME_STATE_LEVEL_SELECT` - Level selection (new)
- `GAME_STATE_LEVEL_PLAY` - Active gameplay (new)

### File Structure

```
include/level_launcher.h     - Level launcher interface
src/level_launcher.c         - Implementation
test_level_launcher.c        - Standalone test utility
test_levels.sh              - Convenience shell script
```

## Build Integration

The level launcher is integrated into the main Makefile:

```makefile
# Build main game (includes level launcher)
make

# Build standalone test utility
make test-launcher

# Clean everything including test launcher
make clean
```

## Testing Framework Features

- **Resource Validation**: Attempts to load proper data##.dll files
- **Progress Simulation**: Creates realistic save progression
- **Boss Detection**: Identifies levels with boss encounters
- **Video Mapping**: Maps intro/boss videos to each level
- **Error Handling**: Graceful fallbacks for missing resources

## Integration with Main Game

The level launcher can be integrated into the main game by:

1. Adding level selection to the title menu
2. Implementing save/load functionality
3. Connecting to the actual level loading system
4. Adding real input handling for level navigation

## Development Notes

- DLL loading currently fails in the test environment (expected behavior)
- Real integration would require connecting to the game's asset loading system
- The framework provides the foundation for cheat codes and debug menus
- All level metadata is derived from evidence-based analysis of game assets

## Future Enhancements

- Real save game loading/saving
- Level completion tracking
- Difficulty selection per level
- Custom equipment loadouts
- Performance benchmarking per level
- Screenshot capture for each level start