#ifndef PRESETS_HPP
#define PRESETS_HPP

#include <iostream>
#include <string>

class Presets {

private:
  std::string name;
  std::string drinkType;

  // coffee values
  std::string coffeeRoast;
  std::string coffeeStrength;
  double coffeeCups = 0.0;

  // latte values
  std::string shotSize;
  int shots = 0;
  std::string latteStrength;
  std::string milkStyle;
  double milkRatio = 0.0;

public:
  // constructor
  Presets(const std::string &name);

  // set
  void setCoffee(const std::string &roast, const std::string &strength,
                 double cups);
  void setLatte(const std::string &shotSize, int shots,
                const std::string &strength, const std::string &milkStyle,
                double ratio);

  // accessors
  std::string getName() const;
  std::string getDrinkType() const;

  // coffee accessors
  std::string getRoast() const;
  std::string getStrength() const;
  double getCups() const;

  // latte accessors
  std::string getShotSize() const;
  int getShots() const;
  std::string getMilkStyle() const;
  double getMilkRatio() const;
  std::string getLatteStrength() const;

  // display
  friend std::ostream &operator<<(std::ostream &out, const Presets &p);
};

#endif
