#ifndef FUNCTIONS_HPP
#define FUNCTIONS_HPP
#include <iostream>
#include <string>

/******************************************************************
 * Struct: CoffeeResult
 * ---------------------------------------------------------------
 * Holds all calculated coffee values (water, beans, ratio, etc.).
 ******************************************************************/
struct CoffeeResult {
  double waterCups;
  double waterML;
  double coffeeGrams;
  double ratio;
  double tablespoons;
};

/******************************************************************
 * Struct: LatteResult
 * ---------------------------------------------------------------
 * Holds all calculated latte/espresso values including shot count,
 * brew ratio, espresso volume, and optional milk volumes.
 ******************************************************************/
struct LatteResult {
  int shots;
  std::string shotSize;
  double coffeeGrams;
  double brewRatio;
  double espressoML;
  double espressoCups;
  double tablespoons;

  bool hasMilkTarget = false;
  std::string milkStyle;
  double milkToEspRatio = 0.0;
  double milkML = 0.0;
  double milkCups = 0.0;
  double finalML = 0.0;
  double finalCups = 0.0;

  // latte constructor
  LatteResult();
  LatteResult(int s, const std::string &size, double grams, double ratio,
              double espML, double espCups, double tbsp);
};

std::string toLowerCopy(std::string s);

double gramsToTablespoons(double grams);

double milkStyleToRatio(const std::string &styleIn);

bool calcCoffee(const std::string &strengthIn, const std::string &roastIn,
                double cups, CoffeeResult &out);

bool calcLatteFromShots(const std::string &strengthIn,
                        const std::string &shotSizeIn, int shotCount,
                        LatteResult &out);

void printCoffeeSummary(const CoffeeResult &r, const std::string &roastType,
                        const std::string &strength);

void printLatteSummary(const LatteResult &r, const std::string &strength,
                       const std::string &shotSize);

#endif
