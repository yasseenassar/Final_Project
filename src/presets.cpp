#include "presets.hpp"
#include <iostream>
using namespace std;

// constructor
Presets::Presets(const string &name) : name(name) {}

// setups
void Presets::setCoffee(const string &roast, const string &strength,
                        double cups) {
  drinkType = "coffee";
  coffeeRoast = roast;
  coffeeStrength = strength;
  coffeeCups = cups;
}

void Presets::setLatte(const string &shotSize, int shots,
                       const string &strength, const string &milkStyle,
                       double ratio) {

  drinkType = "latte";
  this->shotSize = shotSize;
  this->shots = shots;
  this->latteStrength = strength;
  this->milkStyle = milkStyle;
  this->milkRatio = ratio;
}

// accessors
string Presets::getName() const { return name; }

string Presets::getDrinkType() const { return drinkType; }

// coffee accessors
string Presets::getRoast() const { return coffeeRoast; }

string Presets::getStrength() const { return coffeeStrength; }

double Presets::getCups() const { return coffeeCups; }

// latte accessors
string Presets::getShotSize() const { return shotSize; }

int Presets::getShots() const { return shots; }

string Presets::getLatteStrength() const { return latteStrength; }

string Presets::getMilkStyle() const { return milkStyle; }

double Presets::getMilkRatio() const { return milkRatio; }

// display operator
ostream &operator<<(ostream &out, const Presets &p) {
  out << "\n=== Preset: " << p.name << " ===\n";
  out << "Type: " << p.drinkType << "\n";

  // coffee preset
  if (p.drinkType == "coffee") {
    out << "Roast:            " << p.coffeeRoast << "\n";
    out << "Strength:         " << p.coffeeStrength << "\n";
    out << "Cups:             " << p.coffeeCups << "\n";
  }
  // latte preset
  else {
    out << "Shot Size:        " << p.shotSize << "\n";
    out << "Shots:            " << p.shots << "\n";
    out << "Strength:         " << p.latteStrength << "\n";
    out << "Milk Style:       " << p.milkStyle << "\n";
    out << "Milk Ratio:       " << p.milkRatio << "\n";
  }

  return out;
}
