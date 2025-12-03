#include "presetManager.hpp"
#include <iostream>
using namespace std;

// used to put things into the vector
void PresetManager::addPreset(const Presets& preset) {
    presets.push_back(preset);
}

// preset menu display
void PresetManager::listPresets() const {
    if (presets.empty()) {
        cout << "\nNo presets saved.\n";
        return;
    }

    cout << "\n--- Saved Presets ---\n";
    for (const auto& p : presets) {
        cout << "• " << p.getName()
             << " (" << p.getDrinkType() << ")\n";
    }
}


// accessor
Presets* PresetManager::getPresetByName(const string& name) {
    for (auto& p : presets) {
        if (p.getName() == name)
            return &p;
    }
    return nullptr;
}

// to check if list is empty
bool PresetManager::hasPresets() const {
    return !presets.empty();
}

vector<string> PresetManager::getPresetNames() const {
    vector<string> names;
    for (const auto& p : presets) {
        names.push_back(p.getName());
    }
    return names;
}
