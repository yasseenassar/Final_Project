/******************************************************************
 * Simon Fraser University
 * ENSC 151 - Introduction to Software Development for Engineers
 * 
 * Group Project
 * File: coffee_latte_calculator.cpp
 * Author: Alexander Lindsay, Dominick Lum, 
 *         Joshua Mandin, Yasseen Nassar
 *
 * 
 * Program: Coffee & Latte Ratio Calculator (Repeatable Session)
 * 
 * Description:
 *   Interactive calculator that determines coffee/water ratios for brewed coffee
 *   or espresso/milk ratios for lattes. Includes roast-based ratio tweaks,
 *   tablespoons conversion (1 tbsp = 16 g), latte by shots, optional milk target,
 *   and full input validation. Uses a do-while loop to let users
 *   repeat multiple calculations in one session. Additionally, user is able to
 *   create different presets and load them on command.
 * 
 * Input:
 *   - Drink type: coffee or latte
 *   - Roast type (light, medium, dark) for coffee
 *   - Strength (bolder, medium, weaker) for coffee
 *   - Latte strength (stronger or weaker)
 *   - Shot size (single = 8g, double = 16g)
 *   - Number of espresso shots
 *   - Milk style (none, cortado, flatwhite, latte, or custom ratio)
 *   - Repeat prompt (y/n)
 * 
 * Output:
 *   - Required water or espresso volume (cups and mL)
 *   - Coffee grounds (grams and tablespoons)
 *   - Optional milk and final latte size
 ******************************************************************/

#include <iostream>
#include <iomanip>
#include <string>
#include <algorithm>
#include <cctype>    // for std::tolower
#include <limits>    // for std::numeric_limits
using namespace std;

#include "machineDisplay.hpp"

/******************************************************************
 * Function: main
 * ---------------------------------------------------------------
 * Main driver function. Handles user interaction, input validation,
 * and displays results for either coffee or latte calculations.
 * Allows repeating the session via a do-while loop.
 *
 * Returns:
 *   0 on successful execution.
 ******************************************************************/
int main() {
    cout << fixed << setprecision(2);

    char again = 'y';
    
    MachineDisplay m;
    m.run(again);

    cout << "Thank you for using the Coffee & Latte Ratio Calculator!\n";
    return 0;
}