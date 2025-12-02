# ============================================================================
# Makefile for Coffee & Latte Ratio Calculator (Linux / Windows)
# ============================================================================

# Compiler settings
CXX = g++
CXXFLAGS = -std=c++17 -Wall -g -Iinclude

# Directories
SRC_DIR = src
BUILD_DIR = build
BIN_DIR = bin
INCLUDE_DIR = include

# OS-specific tweaks
ifeq ($(OS),Windows_NT)
  EXE_EXT := .exe
  MKDIR_P = if not exist "$(1)" mkdir "$(1)"
  RM_RF = if exist "$(1)" rmdir /S /Q "$(1)"
  RUN_EXEC := $(subst /,\\,$(TARGET))
else
  EXE_EXT :=
  MKDIR_P = mkdir -p "$(1)"
  RM_RF = rm -rf "$(1)"
  RUN_EXEC := ./$(TARGET)
endif

# Project Name (Output path)
TARGET = $(BIN_DIR)/final_project$(EXE_EXT)

# Source files
ALL_SOURCES = $(wildcard $(SRC_DIR)/*.cpp)
# Shared sources exclude both entry points
BASE_SOURCES = $(filter-out $(SRC_DIR)/main.cpp $(SRC_DIR)/gui_main.cpp,$(ALL_SOURCES))
# Console sources include console main
SOURCES = $(BASE_SOURCES) $(SRC_DIR)/main.cpp
# GUI sources include GUI main
GUI_SOURCES = $(BASE_SOURCES) $(SRC_DIR)/gui_main.cpp

# Object files (mapped to build directory)
BASE_OBJECTS = $(BASE_SOURCES:$(SRC_DIR)/%.cpp=$(BUILD_DIR)/%.o)
OBJECTS = $(BASE_OBJECTS) $(BUILD_DIR)/main.o
GUI_OBJECTS = $(BASE_OBJECTS) $(BUILD_DIR)/gui_main.o

# ============================================================================
# Targets
# ============================================================================

# Default target
all: $(TARGET)

# Link the executable
$(TARGET): $(OBJECTS) | $(BIN_DIR)
	@echo "Linking executable..."
	$(CXX) $(CXXFLAGS) -o $@ $^
	@echo "Build complete: $@"

# Compile source files into object files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp | $(BUILD_DIR)
	@echo "Compiling $<..."
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Create directories if they don't exist
$(BIN_DIR) $(BUILD_DIR):
	@$(call MKDIR_P,$@)

# Build and run the program
run: $(TARGET)
	@echo "Running $(TARGET)..."
	@$(RUN_EXEC)

# GUI build (requires SFML)
GUI_LIBS ?= -lsfml-graphics -lsfml-window -lsfml-system
GUI_TARGET = $(BIN_DIR)/final_project_gui$(EXE_EXT)
ifeq ($(OS),Windows_NT)
  GUI_RUN := $(subst /,\\,$(GUI_TARGET))
else
  GUI_RUN := ./$(GUI_TARGET)
endif

gui: $(GUI_TARGET)

$(GUI_TARGET): $(GUI_OBJECTS) | $(BIN_DIR)
	@echo "Linking GUI executable..."
	$(CXX) $(CXXFLAGS) -o $@ $(GUI_OBJECTS) $(GUI_LIBS)
	@echo "GUI build complete: $@"

# Build and run GUI
run-gui: $(GUI_TARGET)
	@echo "Running $(GUI_TARGET)..."
	@$(GUI_RUN)

# Clean up build files
clean:
	@echo "Cleaning build artifacts..."
	@$(call RM_RF,$(BUILD_DIR))
	@$(call RM_RF,$(BIN_DIR))
	@echo "Clean complete."

# Display help information
help:
	@echo "Coffee & Latte Ratio Calculator - Makefile Help"
	@echo "================================================"
	@echo ""
	@echo "Linux/macOS:"
	@echo "  make            - Build the project (default)"
	@echo "  make run        - Build and run (./bin/final_project)"
	@echo "  make clean      - Remove all build artifacts"
	@echo "  make gui        - Build GUI version (requires SFML)"
	@echo "  make run-gui    - Build and run GUI (./bin/final_project_gui)"
	@echo "  make help       - Display this help message"
	@echo ""
	@echo "Windows (MinGW):"
	@echo "  mingw32-make    - Build the project (default)"
	@echo "  mingw32-make run    - Build and run (bin/final_project.exe)"
	@echo "  mingw32-make clean  - Remove all build artifacts"
	@echo "  mingw32-make gui    - Build GUI version (requires SFML)"
	@echo "  mingw32-make run-gui - Build and run GUI (bin/final_project_gui.exe)"
	@echo ""
	@echo "Notes:"
	@echo "  GUI requires SFML 2.5+ installed."
	@echo "  Windows GUI run example: .\\bin\\final_project_gui.exe"
	@echo ""
	@echo "Project structure:"
	@echo "  src/          - Source files (.cpp)"
	@echo "  include/      - Header files (.hpp)"
	@echo "  build/        - Object files (.o)"
	@echo "  bin/          - Final executable"
	@echo ""

.PHONY: all run clean help
