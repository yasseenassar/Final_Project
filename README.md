# Coffee & Latte Ratio Calculator

## Description
Interactive calculator that determines coffee/water ratios for brewed coffee or espresso/milk ratios for lattes. Includes roast-based ratio tweaks, tablespoons conversion, latte by shots, optional milk target, and full input validation.

## Authors
- Alexander Lindsay
- Dominick Lum
- Joshua Mandin
- Yasseen Nassar

**Course:** ENSC 151 - Introduction to Software Development for Engineers  
**Institution:** Simon Fraser University

## Features
- Coffee brewing ratio calculator with roast and strength customization
- Latte ratio calculator with espresso shots and milk options
- Support for different milk styles (cortado, flat white, latte, custom)
- Preset system to save and load favorite drink configurations
- **Daily Caffeine Stats**: Tracks estimated caffeine intake for the session
- **High Caffeine Warning**: Alerts when ordering more than 5 espresso shots
- Input validation and error handling
- Resizable GUI window with responsive control tips

## Building the Project

### Option 1: Direct `g++` (no make required)
- Linux/macOS console (exclude GUI main): `g++ -std=c++17 -Wall -g -Iinclude src/main.cpp src/functions.cpp src/machineDisplay.cpp src/presets.cpp src/presetsManager.cpp -o final_project`
- Linux/macOS GUI (requires SFML): `g++ -std=c++17 -Wall -g -Iinclude src/gui_main.cpp src/functions.cpp src/machineDisplay.cpp src/presets.cpp src/presetsManager.cpp -lsfml-graphics -lsfml-window -lsfml-system -o final_project_gui`
- Windows console (MSYS2 MinGW64 shell): `g++ -std=c++17 -Wall -g -Iinclude src/main.cpp src/functions.cpp src/machineDisplay.cpp src/presets.cpp src/presetsManager.cpp -o final_project.exe`
- Windows GUI (MSYS2 MinGW64 shell, SFML installed): `g++ -std=c++17 -Wall -g -Iinclude src/gui_main.cpp src/functions.cpp src/machineDisplay.cpp src/presets.cpp src/presetsManager.cpp -lsfml-graphics -lsfml-window -lsfml-system -o final_project_gui.exe`

### Option 2: Using Make (requires make/mingw32-make; creates bin/build folders)
- Linux/macOS:
  - `make` (build console)
  - `make run` (build + run console)
  - `make gui` / `make run-gui` (build + run GUI, needs SFML)
  - `make clean` (remove build artifacts)
  - `make help` (targets summary)
- Windows (PowerShell or cmd in MSYS2 MinGW64 shell):
  - `mingw32-make` (build console) -> `bin\final_project.exe`
  - `mingw32-make run`
  - `mingw32-make gui` / `mingw32-make run-gui` (GUI, needs SFML)
  - `mingw32-make clean`

### Requirements
- C++ compiler with C++17 support (g++ or MinGW-w64 g++)
- GNU Make (Linux/macOS) or mingw32-make (Windows) if using the Makefile
- Optional for GUI: SFML 3.0+ (libsfml-dev on Ubuntu/Debian, Homebrew `sfml` on macOS, MSYS2/mingw-w64 packages on Windows)

Install SFML (GUI only):
- Ubuntu/Debian: `sudo apt-get update && sudo apt-get install -y libsfml-dev`
- macOS (Homebrew): `brew install sfml`
- Windows (MSYS2 MinGW64 shell): `pacman -S mingw-w64-x86_64-sfml`

### Environment Setup

Linux (Ubuntu/Debian):
- Install build tools:
  - `sudo apt-get update`
  - `sudo apt-get install -y build-essential`
- Optional GUI (SFML 3.0+):
  - `sudo apt-get install -y libsfml-dev`

macOS:
- Install Xcode Command Line Tools:
  - `xcode-select --install`
- Optional GUI (SFML 3.0+ via Homebrew):
  - `brew install sfml`

Windows (MSYS2 MinGW64 shell):
- Install toolchain:
  - `pacman -S --needed base-devel mingw-w64-x86_64-toolchain`
- Optional GUI (SFML 3.0+):
  - `pacman -S mingw-w64-x86_64-sfml`

## Running the Program
```bash
# Linux/macOS console
./bin/final_project

# Linux/macOS GUI (after SFML install)
./bin/final_project_gui

# Windows console (cmd/PowerShell)
.\bin\final_project.exe

# Windows GUI (cmd/PowerShell, after SFML install)
.\bin\final_project_gui.exe
```

## Project Structure
```
Final_Project/
├── bin/           # Final executable
├── build/         # Object files (.o)
├── include/       # Header files (.hpp)
├── src/           # Source files (.cpp)
├── resources/     # Assets (fonts)
├── Makefile       # Build configuration
└── README.md      # This file
```

## Usage
The program presents an interactive menu where you can:
1. **Make a drink** - Calculate ratios for coffee or latte
2. **Create a preset** - Save your favorite drink configuration
3. **Load a preset** - Quickly recreate a saved drink

Follow the on-screen prompts to customize your drink parameters.

## How It Works

### Terminal Interface (text UI)
- Entry point: `src/main.cpp` creates a `MachineDisplay` and calls `run()`.
- `MachineDisplay` (in `src/machineDisplay.cpp`):
  - Shows the main menu (make a drink, create a preset, load a preset).
  - For coffee: asks roast (light/medium/dark), strength (bolder/medium/weaker), cups; calls `calcCoffee` from `functions.cpp`; prints a summary.
  - For latte: asks strength (stronger/weaker), shot size (single/double), number of shots, milk style (none/cortado/flatwhite/latte/custom); optional custom milk ratio; calls `calcLatteFromShots`; prints a summary.
  - Presets: `createPreset` collects drink inputs and stores via `PresetManager`; `loadPreset` finds a preset, recalculates, and prints the summary.
- Core math: `functions.cpp` provides `calcCoffee`, `calcLatteFromShots`, conversions, and summary printers. Constants live in `constants.hpp`.
- Presets: `Presets` stores one configuration; `PresetManager` holds many and can list or fetch by name.

### GUI Interface (SFML)
- Entry: `src/gui_main.cpp` with `runGui()` and `main()`.
- Navigation: arrow keys move selection; Enter confirms; Esc returns to main menu. Number fields (cups, shots, custom ratio) use Up/Down to adjust; text entry for preset names.
- Back: Left arrow moves to the previous step while keeping your current selections/inputs.
- Flow mirrors the terminal:
  - Main menu: Make a drink, Create preset, Load preset, Quit (Quit closes the window).
  - Coffee flow: drink type → roast → strength → cups → summary.
  - Latte flow: drink type → strength → shot size → shots → milk style (custom prompts ratio) → summary.
  - Presets: can create (coffee or latte) and save; load lists saved presets and recalculates summaries.
- Rendering: simple centered text, highlighted selections. The window closes immediately when Quit is confirmed.

### Data Structures
- `CoffeeResult` and `LatteResult` structs carry computed values for summaries.
- `Presets` holds a saved drink; `PresetManager` manages a vector of presets and exposes names for GUI loading.
- `constants.hpp` defines shared constants (e.g., mL per cup, grams per tablespoon).

## Recent Changes
- Cross-platform Makefile: Windows/Linux support with GUI targets; MinGW/SFML documented.
- Header hygiene: removed `using namespace std` from headers and unused includes to avoid conflicts.
- Input validation: added checks for menu/preset inputs; latte summary shows shot size correctly.
- GUI: Updated for SFML 3.0 compatibility; enabled window resizing; added caffeine stats and warnings; implemented dynamic vertical centering.
- Font: Bundled `arial.ttf` in `resources/` for consistent cross-platform appearance; removed dependency on system fonts.
- Preset manager: added `getPresetNames()` to list presets for GUI loading.
- Docs/help: README and `make help` now include OS-specific build/run commands and copy-paste setup for Linux and Windows.
- Cleanup: removed obsolete root-level duplicates of source/header files; canonical code lives in `src/` and `include/`.
