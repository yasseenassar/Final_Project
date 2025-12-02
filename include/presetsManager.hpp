#ifndef PRESETMANAGER_HPP
#define PRESETMANAGER_HPP

#include "presets.hpp"
#include <string>
#include <vector>

class PresetManager {

private:
  std::vector<Presets> presets;

public:
  void addPreset(const Presets &preset);
  void listPresets() const;
  Presets *getPresetByName(const std::string &name);

  bool hasPresets() const;
  std::vector<std::string> getPresetNames() const;
};

#endif
