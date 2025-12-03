#ifndef PRESETS_HPP
#define PRESETS_HPP

#include <string>
#include <iostream>
using namespace std;

class Presets{
    
    private:
    
        string name;
        string drinkType;
        
        // coffee values
        string coffeeRoast;
        string coffeeStrength;
        double coffeeCups = 0.0;
        
        // latte values
        string shotSize;
        int shots = 0;
        string latteStrength;
        string milkStyle;
        double milkRatio = 0.0;
        
    public:
    
        // constructor
        Presets(const string& name);
        
        // set
        void setCoffee(const string& roast, const string& strength, double cups);
        void setLatte(const string& shotSize, int shorts, const string& strength,
                       const string& milkStyle, double ratio);
                       
        // accessors
        string getName() const;
        string getDrinkType() const;
        
        // coffee accessors
        string getRoast() const;
        string getStrength() const;
        double getCups() const;
        
        // latte accessors
        string getShotSize() const;
        int getShots() const;
        string getMilkStyle() const;
        double getMilkRatio() const;
        string getLatteStrength() const;
        
    
        // display
        friend ostream& operator<<(ostream& out, const Presets& p);
        
};




#endif