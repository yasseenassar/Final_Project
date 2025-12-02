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
- Input validation and error handling

## Building the Project

### Requirements
- C++ compiler with C++17 support (g++ or MinGW-w64 g++)
- GNU Make (Linux/macOS) or mingw32-make (Windows)
- Optional for GUI: SFML 2.5+ (libsfml-dev on Ubuntu/Debian, MSYS2/mingw-w64
  packages on Windows)

Install SFML (GUI only):
- Ubuntu/Debian: `sudo apt-get update && sudo apt-get install -y libsfml-dev`
- Windows (MSYS2 MinGW64 shell): `pacman -S mingw-w64-x86_64-sfml`

### Environment Setup

Linux (Ubuntu/Debian):
- Install build tools:
  - `sudo apt-get update`
  - `sudo apt-get install -y build-essential`
- Optional GUI (SFML 2.5+):
  - `sudo apt-get install -y libsfml-dev`

Windows (MSYS2 MinGW64 shell):
- Install toolchain:
  - `pacman -S --needed base-devel mingw-w64-x86_64-toolchain`
- Optional GUI (SFML 2.5+):
  - `pacman -S mingw-w64-x86_64-sfml`

### Build Commands
```bash
make            # Compile the project (Linux/macOS/Git Bash)
make clean      # Remove all build artifacts
make run        # Build and run the program
make help       # Show available targets

# Windows (PowerShell or cmd with MinGW-w64)
mingw32-make    # Build -> bin\final_project.exe
mingw32-make run
mingw32-make clean

# Optional SFML GUI window (after installing SFML):
make gui        # Build -> bin/final_project_gui
make run-gui    # Build and run GUI
# Windows: mingw32-make gui / mingw32-make run-gui
# Windows run command: .\bin\final_project_gui.exe

### Modes
- Terminal mode: `make run` (Linux/macOS) or `mingw32-make run` (Windows) to use the text UI.
- GUI mode: `make run-gui` (Linux/macOS) or `mingw32-make run-gui` (Windows) after SFML is installed.
```

## Running the Program
```bash
./bin/final_project
# Windows
.\bin\final_project.exe
```

## Project Structure
```
Final_Project/
├── bin/           # Final executable
├── build/         # Object files (.o)
├── include/       # Header files (.hpp)
├── src/           # Source files (.cpp)
├── Makefile       # Build configuration
└── README.md      # This file
```

## Usage
The program presents an interactive menu where you can:
1. **Make a drink** - Calculate ratios for coffee or latte
2. **Create a preset** - Save your favorite drink configuration
3. **Load a preset** - Quickly recreate a saved drink

Follow the on-screen prompts to customize your drink parameters.

## Recent Changes
- Cross-platform Makefile: Windows/Linux support with GUI targets; MinGW/SFML documented.
- Header hygiene: removed `using namespace std` from headers and unused includes to avoid conflicts.
- Input validation: added checks for menu/preset inputs; latte summary shows shot size correctly.
- GUI: SFML interface with arrow-key navigation mirroring terminal flow; supports creating and loading presets; centered prompts/summaries.
- Preset manager: added `getPresetNames()` to list presets for GUI loading.
- Docs/help: README and `make help` now include OS-specific build/run commands and copy-paste setup for Linux and Windows.
