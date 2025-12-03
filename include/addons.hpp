#ifndef ADDONS_HPP
#define ADDONS_HPP

#include "functions.hpp"
#include "constants.hpp"
#include <iostream>
#include <string>
using namespace std;

// Handles add-ons for coffee drinks
void handleCoffeeAddOns(CoffeeResult& coffee);

// Applies add-on logic for coffee (GUI/Console shared)
void applyCoffeeAddons(CoffeeResult& coffee, bool isIced, const std::string& topping);

// Handles add-ons for latte drinks
void handleLatteAddOns(LatteResult& latte);

// Applies add-on logic for latte (GUI/Console shared)
void applyLatteAddons(LatteResult& latte, bool isIced, int extraShots, const std::string& topping);

#endif
