#ifndef MACHINEDISPLAY_HPP
#define MACHINEDISPLAY_HPP

#include "functions.hpp"
#include "constants.hpp"
#include "presetManager.hpp"

class MachineDisplay {
    
    private:
    
        void handleCoffee();
        void handleLatte();
        
        // for presets
        void createPreset();
        void loadPreset();
        PresetManager presetManager;
        
    public:
    
        void run(char &again);
        char askRepeat(char &again);
    
};





#endif