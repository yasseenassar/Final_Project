#ifndef PRESETMANAGER_HPP
#define PRESETMANAGER_HPP

#include <vector>
#include <string>
#include "presets.hpp"
using namespace std;

class PresetManager {
    
    private:
    
        vector<Presets> presets;

    public:
    
    // list functions
        void addPreset(const Presets& preset);
        void listPresets() const;
        
    // accessor
        Presets* getPresetByName(const string& name);
        vector<string> getPresetNames() const;
    
        bool hasPresets() const;


};

#endif
