#include "machineDisplay.hpp"
#include "constants.hpp"
#include <iostream>
#include <limits> // for std::numeric_limits

using namespace std;

// displays the interface for the user to select: coffee or latte
void MachineDisplay::run(char &again) {

  while (again == 'y') {

    cout << "\n=== Coffee & Latte Ratio Calculator ===";

    int option;

    cout << "\n            === Menu ===\n"
         << "          1) Make a drink\n"
         << "          2) Create a preset\n"
         << "          3) Load a preset\n\n"
         << "Enter option: ";

    cin >> option;

    // Input validation
    if (cin.fail()) {
      cin.clear();
      cin.ignore(numeric_limits<streamsize>::max(), '\n');
      cout << "\nInvalid input. Please enter a number.\n";
      continue;
    }

    switch (option) {

    case 1: {

      bool valid = false;
      string drinkType;

      // Choose drink type
      while (!valid) {
        cout << "Choose drink (coffee or latte): ";
        cin >> drinkType;
        string d = toLowerCopy(drinkType);
        if (d == "coffee" || d == "latte")
          valid = true;
        else
          cout << "Invalid input. Please enter 'coffee' or 'latte'.\n";
      }
      string drink = toLowerCopy(drinkType);

      if (drink == "coffee")
        handleCoffee();
      else
        handleLatte();

      break;
    }
    case 2: {
      createPreset();
      break;
    }
    case 3: {
      loadPreset();
      break;
    }
    default: {
      cout << "\nPlease enter a valid option!\n";
      break;
    }
    }

    again = askRepeat(again);
  }
}

// if the user selects coffee
void MachineDisplay::handleCoffee() {

  string roastType, strength;
  double cups = 0.0;

  // Roast selection
  bool validRoast = false;
  while (!validRoast) {
    cout << "Enter bean roast type (light, medium, dark): ";
    cin >> roastType;
    string r = toLowerCopy(roastType);
    if (r == "light" || r == "medium" || r == "dark")
      validRoast = true;
    else
      cout << "Invalid roast. Please enter light, medium, or dark.\n";
  }

  // Strength
  bool validStrength = false;
  while (!validStrength) {
    cout << "Enter coffee strength (bolder, medium, weaker): ";
    cin >> strength;
    string s = toLowerCopy(strength);
    if (s == "bolder" || s == "medium" || s == "weaker")
      validStrength = true;
    else
      cout << "Invalid strength. Please enter bolder, medium, or weaker.\n";
  }

  // Cups (numeric do-while)
  bool validCups;
  do {
    validCups = true;
    cout << "Enter desired coffee amount (in cups): ";
    cin >> cups;
    if (cin.fail() || cups <= 0) {
      cin.clear();
      cin.ignore(numeric_limits<streamsize>::max(), '\n');
      cout << "Invalid amount. Please enter a positive number.\n";
      validCups = false;
    }
  } while (!validCups);

  CoffeeResult r;
  if (calcCoffee(strength, roastType, cups, r)) {

    printCoffeeSummary(r, roastType, strength);

  } else {
    cout << "Calculation error.\n";
  }
}

// if the user selects latte
void MachineDisplay::handleLatte() {
  // LATTE SECTION
  string strength, shotSize;
  int shots = 0;

  // Strength selection
  bool validLStrength = false;
  string calcStrength;

  while (!validLStrength) {
    cout << "Enter latte strength (stronger or weaker): ";
    cin >> strength;

    string s = toLowerCopy(strength);

    if (s == "stronger" || s == "weaker") {
      calcStrength = s;
      validLStrength = true;
    } else {
      cout << "Invalid input. Please enter 'stronger' or 'weaker'.\n";
    }
  }

  // Shot size
  bool validShotSize = false;
  while (!validShotSize) {
    cout << "Shot size (single = 8g, double = 16g): ";
    cin >> shotSize;
    string ss = toLowerCopy(shotSize);
    if (ss == "single" || ss == "double")
      validShotSize = true;
    else
      cout << "Invalid shot size. Enter single or double.\n";
  }

  // Number of shots
  bool validShots;
  do {
    validShots = true;
    cout << "How many shots? ";
    cin >> shots;
    if (cin.fail() || shots <= 0) {
      cin.clear();
      cin.ignore(numeric_limits<streamsize>::max(), '\n');
      cout << "Invalid input. Enter a positive whole number.\n";
      validShots = false;
    }
  } while (!validShots);

  LatteResult r;
  if (!calcLatteFromShots(strength, shotSize, shots, r)) {
    cout << "Calculation error.\n";
    return;
  }

  // Milk style selection
  string milkStyle;
  double ratio = 0.0;
  bool validMilkStyle = false;
  while (!validMilkStyle) {
    cout << "Milk target style (none/cortado/flatwhite/latte/custom): ";
    cin >> milkStyle;
    ratio = milkStyleToRatio(milkStyle);
    if (ratio == -2.0)
      cout << "Invalid milk style. Try again.\n";
    else
      validMilkStyle = true;
  }

  // Custom ratio entry
  if (ratio == -1.0) {
    bool validCustom;
    do {
      validCustom = true;
      cout << "Enter milk:espresso ratio (e.g., 2.5 means milk is 2.5x "
              "espresso): ";
      cin >> ratio;
      if (cin.fail() || ratio < 0.0) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Invalid ratio. Enter a non-negative number.\n";
        validCustom = false;
      }
    } while (!validCustom);
  }

  if (toLowerCopy(milkStyle) != "none") {
    r.hasMilkTarget = true;
    r.milkStyle = milkStyle;
    r.milkToEspRatio = ratio;
    r.milkML = r.espressoML * r.milkToEspRatio;
    r.milkCups = r.milkML / ML_PER_CUP;
    r.finalML = r.espressoML + r.milkML;
    r.finalCups = r.finalML / ML_PER_CUP;
  }

  printLatteSummary(r, calcStrength, shotSize);
}

// prompts the user if they want to continue or not
char MachineDisplay::askRepeat(char &again) {

  // Repeat prompt (validated)
  do {
    cout << "\nWould you like to perform another calculation? (y/n): ";
    cin >> again;
    again = static_cast<char>(tolower(static_cast<unsigned char>(again)));
    if (again != 'y' && again != 'n') {
      cout << "Please enter 'y' or 'n'.\n";
    }
  } while (again != 'y' && again != 'n');

  return again;
}

void MachineDisplay::createPreset() {

  cout << "\n=== Create Preset ===\n";

  string presetName;
  cout << "Enter a name for this preset: ";
  cin >> presetName;

  Presets preset(presetName);

  string drinkChoice;
  cout << "Is this a coffee or latte preset? ";
  cin >> drinkChoice;

  drinkChoice = toLowerCopy(drinkChoice);

  if (drinkChoice == "coffee") {

    string roast, strength;
    double cups;

    cout << "Enter roast (light/medium/dark): ";
    cin >> roast;

    cout << "Enter strength (bolder/medium/weaker): ";
    cin >> strength;

    cout << "Enter cups: ";
    cin >> cups;

    if (cin.fail() || cups <= 0) {
      cin.clear();
      cin.ignore(numeric_limits<streamsize>::max(), '\n');
      cout << "Invalid input. Preset not saved.\n";
      return;
    }

    preset.setCoffee(roast, strength, cups);
  } else if (drinkChoice == "latte") {

    string shotSize, strength, milk;
    int shots;
    double ratio = 0.0;

    cout << "Enter shot size (single/double): ";
    cin >> shotSize;

    cout << "Enter number of shots: ";
    cin >> shots;

    if (cin.fail() || shots <= 0) {
      cin.clear();
      cin.ignore(numeric_limits<streamsize>::max(), '\n');
      cout << "Invalid input. Preset not saved.\n";
      return;
    }

    cout << "Enter latte strength (stronger/weaker): ";
    cin >> strength;

    cout << "Milk style (none/cortado/latte/flatwhite/custom): ";
    cin >> milk;

    string milkLower = toLowerCopy(milk);
    if (milkLower == "custom") {
      cout << "Enter milk ratio: ";
      cin >> ratio;

      if (cin.fail() || ratio < 0) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Invalid input. Preset not saved.\n";
        return;
      }
    }

    preset.setLatte(shotSize, shots, strength, milk, ratio);
  } else {
    cout << "Invalid type.\n";
    return;
  }

  presetManager.addPreset(preset);
  cout << "Preset saved!\n";
}

void MachineDisplay::loadPreset() {

  if (!presetManager.hasPresets()) {
    cout << "\nNo presets saved yet.\n";
    return;
  }

  presetManager.listPresets();

  string name;
  cout << "\nEnter preset name: ";
  cin >> name;

  Presets *p = presetManager.getPresetByName(name);

  if (!p) {
    cout << "Preset not found.\n";
    return;
  }

  // Show preset data using your friend operator<<
  cout << *p;

  cout << "\n=== Calculated Drink Summary ===\n";

  // COFFEE PRESET
  if (p->getDrinkType() == "coffee") {

    CoffeeResult r;

    bool ok = calcCoffee(p->getStrength(), p->getRoast(), p->getCups(), r);

    if (!ok) {
      cout << "Error calculating preset.\n";
      return;
    }

    printCoffeeSummary(r, p->getRoast(), p->getStrength());
  }

  // LATTE PRESET
  else {

    LatteResult r;

    bool ok = calcLatteFromShots(p->getLatteStrength(), p->getShotSize(),
                                 p->getShots(), r);

    if (!ok) {
      cout << "Error calculating preset.\n";
      return;
    }

    // Apply milk style (if not "none")
    if (toLowerCopy(p->getMilkStyle()) != "none") {

      r.hasMilkTarget = true;
      r.milkStyle = p->getMilkStyle();
      r.milkToEspRatio = p->getMilkRatio();

      r.milkML = r.espressoML * r.milkToEspRatio;
      r.milkCups = r.milkML / ML_PER_CUP;
      r.finalML = r.espressoML + r.milkML;
      r.finalCups = r.finalML / ML_PER_CUP;
    }

    printLatteSummary(r, p->getLatteStrength(), p->getShotSize());
  }
}
