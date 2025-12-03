#include "functions.hpp"
#include "constants.hpp"
#include <iostream>
using namespace std;

/******************************************************************
 * Function: toLowerCopy
 * ---------------------------------------------------------------
 * Converts all characters in a given string to lowercase.
 *
 * Parameters:
 *   s - the string to convert
 *
 * Returns:
 *   The lowercase version of the input string.
 ******************************************************************/
string toLowerCopy(string s) {
    transform(s.begin(), s.end(), s.begin(),
              [](unsigned char c){ return static_cast<char>(tolower(c)); });
    return s;
}

/******************************************************************
 * Function: gramsToTablespoons
 * ---------------------------------------------------------------
 * Converts grams of coffee into tablespoons.
 *
 * Parameters:
 *   grams - amount of coffee in grams
 *
 * Returns:
 *   Equivalent tablespoons of coffee grounds.
 ******************************************************************/
double gramsToTablespoons(double grams) {
    return grams / GRAMS_PER_TBSP;
}

/******************************************************************
 * Function: milkStyleToRatio
 * ---------------------------------------------------------------
 * Maps a milk style keyword to its corresponding milk-to-espresso
 * ratio. Returns -1 for custom, -2 for invalid style.
 *
 * Parameters:
 *   styleIn - user-entered milk style string
 *
 * Returns:
 *   Ratio as a double, or a negative code for special handling.
 ******************************************************************/
double milkStyleToRatio(const string& styleIn) {
    string s = toLowerCopy(styleIn);
    if (s == "cortado") return 1.0;
    if (s == "flatwhite" || s == "flat_white") return 2.0;
    if (s == "latte") return 3.0;
    if (s == "none") return 0.0;
    if (s == "custom") return -1.0;
    return -2.0;
}

/******************************************************************
 * Function: calcCoffee
 * ---------------------------------------------------------------
 * Calculates required coffee grinds and water amounts based on
 * strength, roast type, and number of cups desired.
 *
 * Parameters:
 *   strengthIn - coffee strength (bolder, medium, weaker)
 *   roastIn    - roast type (light, medium, dark)
 *   cups       - desired number of cups
 *   out        - reference to CoffeeResult struct for storing results
 *
 * Returns:
 *   true if calculation successful, false if invalid input.
 ******************************************************************/
bool calcCoffee(const string& strengthIn, const string& roastIn,
                double cups, CoffeeResult& out) {
    string strength = toLowerCopy(strengthIn);
    string roast    = toLowerCopy(roastIn);

    double baseRatio = 0.0;
    if (strength == "bolder") baseRatio = 14.0;
    else if (strength == "medium") baseRatio = 16.0;
    else if (strength == "weaker") baseRatio = 18.0;
    else return false;

    if (roast == "light") baseRatio += 1.0;
    else if (roast == "dark") baseRatio -= 1.0;

    if (cups <= 0.0) return false;

    double waterML = cups * ML_PER_CUP;
    double grams   = waterML / baseRatio;
    double tbsp    = gramsToTablespoons(grams);

    out = {cups, waterML, grams, baseRatio, tbsp};
    return true;
}

/******************************************************************
 * Function: calcLatteFromShots
 * ---------------------------------------------------------------
 * Calculates espresso and optional milk volumes for a latte based
 * on shot size, shot count, and brew strength.
 *
 * Parameters:
 *   strengthIn - latte brew strength (stronger or weaker)
 *   shotSizeIn - shot size (single/double)
 *   shotCount  - number of espresso shots
 *   out        - reference to LatteResult struct for storing results
 *
 * Returns:
 *   true if calculation successful, false if invalid input.
 ******************************************************************/
bool calcLatteFromShots(const string& strengthIn, const string& shotSizeIn,
                        int shotCount, LatteResult& out) {
    string strength = toLowerCopy(strengthIn);
    string shotSize = toLowerCopy(shotSizeIn);

    if (shotCount <= 0) return false;

    double gramsPerShot = 0.0;
    if (shotSize == "single")      gramsPerShot = 8.0;
    else if (shotSize == "double") gramsPerShot = 16.0;
    else return false;

    double brewRatio = 0.0;
    if (strength == "stronger")      brewRatio = 2.0;
    else if (strength == "weaker")   brewRatio = 2.5;
    else return false;

    double totalGrams   = gramsPerShot * shotCount;
    double espressoML   = totalGrams * brewRatio;
    double espressoCups = espressoML / ML_PER_CUP;
    double tbsp         = gramsToTablespoons(totalGrams);

    out = LatteResult(shotCount, shotSizeIn, totalGrams,
                  brewRatio, espressoML, espressoCups, tbsp);

    return true;
}


// constructors

LatteResult::LatteResult() 
    : shots(0), shotSize(""), coffeeGrams(0.0), brewRatio(0.0),
      espressoML(0.0), espressoCups(0.0), tablespoons(0.0),
      hasMilkTarget(false), milkStyle(""), milkToEspRatio(0.0),
      milkML(0.0), milkCups(0.0), finalML(0.0), finalCups(0.0)
{}

LatteResult::LatteResult(int s, const std::string& size, double grams,
            double ratio, double espML, double espCups, double tbsp)
    : shots(s), shotSize(size), coffeeGrams(grams), brewRatio(ratio),
      espressoML(espML), espressoCups(espCups), tablespoons(tbsp),
      hasMilkTarget(false), milkStyle(""), milkToEspRatio(0.0),
      milkML(0.0), milkCups(0.0), finalML(0.0), finalCups(0.0)
{}

void printCoffeeSummary(const CoffeeResult& r,
                        const string& roastType,
                        const string& strength)
{
    cout << "\n--- Coffee Summary ---\n";
    cout << "Roast Type:         " << roastType << '\n';
    cout << "Strength:           " << strength << " (1:" << r.ratio << ")\n";
    cout << "Required Water:     " << r.waterCups << " cup(s) (" << r.waterML << " mL)\n";
    cout << "Coffee Grounds:     " << r.coffeeGrams << " g (" << r.tablespoons << " tbsp)\n";
    cout << "Enjoy your coffee!\n";
}

void printLatteSummary(const LatteResult& r,
                       const string& strength,
                       const string& shotSize)
{
    cout << "\n--- Latte Summary ---\n";
    cout << "Strength (brew):      " << strength << " (espresso 1:" << r.brewRatio << ")\n";
    cout << "Shot Plan:            " << r.shots << " x " << r.shotSize << " shot(s)\n";
    cout << "Coffee Grounds:       " << r.coffeeGrams  << " g (" << r.tablespoons << " tbsp)\n";
    cout << "Espresso Output:      " << r.espressoCups << " cup(s) (" << r.espressoML << " mL)\n";

    if (r.hasMilkTarget) {
        cout << "Milk Style Target:    " << r.milkStyle
             << " (milk:espresso = " << r.milkToEspRatio << ":1)\n";
        cout << "Milk Volume:          " << r.milkCups << " cup(s) (" << r.milkML << " mL)\n";
        cout << "Estimated Final Size: " << r.finalCups << " cup(s) (" << r.finalML << " mL)\n";
    } else {
        cout << "Milk:                 to taste (add milk to reach your preferred latte size)\n";
    }

    cout << "Enjoy your latte!\n";
}

