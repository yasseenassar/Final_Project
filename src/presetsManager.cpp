#include "presetsManager.hpp"
#include <iostream>
using namespace std;

void PresetManager::addPreset(const Presets &preset) {
  presets.push_back(preset);
}

void PresetManager::listPresets() const {
  if (presets.empty()) {
    cout << "\nNo presets saved.\n";
    return;
  }

  cout << "\n--- Saved Presets ---\n";
  for (const auto &p : presets) {
    cout << "• " << p.getName() << " (" << p.getDrinkType() << ")\n";
  }
}

Presets *PresetManager::getPresetByName(const std::string &name) {
  for (auto &p : presets) {
    if (p.getName() == name)
      return &p;
  }
  return nullptr;
}

bool PresetManager::hasPresets() const { return !presets.empty(); }

std::vector<std::string> PresetManager::getPresetNames() const {
  std::vector<std::string> names;
  names.reserve(presets.size());
  for (const auto &p : presets)
    names.push_back(p.getName());
  return names;
}
