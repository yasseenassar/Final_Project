#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>
#include <string>
#include <vector>
#include <sstream>
#include <iomanip>
#include <iostream>
#include <algorithm>
#include <functional>

#include "functions.hpp"
#include "presets.hpp"
#include "presetManager.hpp"
#include "addons.hpp"
#include "constants.hpp"

namespace {

enum class Flow { None, Make, CreatePreset, LoadPreset };

enum class Screen {
  MainMenu,
  ChooseDrink,
  CoffeeRoast,
  CoffeeStrength,
  CoffeeCups,
  LatteStrength,
  LatteShotSize,
  LatteShots,
  LatteMilkStyle,
  LatteMilkRatio,
  PresetName,
  LoadPresetList,
  AddonsPrompt,
  AddonsList,
  ToppingInput,
  ExtraShotsInput,
  Summary,
  Error
};

/******************************************************************
 * Function: formatDouble
 * ---------------------------------------------------------------
 * Formats a double to 2 decimal places.
 ******************************************************************/
std::string formatDouble(double x) {
  std::ostringstream oss;
  oss << std::fixed << std::setprecision(2) << x;
  return oss.str();
}

/******************************************************************
 * Function: loadFont
 * ---------------------------------------------------------------
 * Attempts to load a font from common system locations.
 *
 * Returns true on success.
 ******************************************************************/
bool loadFont(sf::Font &font) {
  const std::vector<std::string> candidates = {
      "resources/DejaVuSans.ttf",
      "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf",
      "/usr/local/share/fonts/DejaVuSans.ttf",
      "C:\\\\Windows\\\\Fonts\\\\arial.ttf"};
  for (const std::string &path : candidates) {
    if (font.openFromFile(path))
      return true;
  }
  return false;
}

/******************************************************************
 * Function: makeText
 * ---------------------------------------------------------------
 * Builds an SFML text object with the default light color.
 ******************************************************************/
sf::Text makeText(const sf::Font &font, const std::string &str, unsigned size) {
  sf::Text t(font, str, size);
  t.setFillColor(sf::Color(240, 240, 240));
  return t;
}

/******************************************************************
 * Function: centerHoriz
 * ---------------------------------------------------------------
 * Centers a text horizontally at the given y on a window of width.
 ******************************************************************/
void centerHoriz(sf::Text &text, float y, float width) {
  sf::FloatRect b = text.getLocalBounds();
  text.setOrigin({b.position.x + b.size.x / 2.0f, b.position.y + b.size.y / 2.0f});
  text.setPosition({width / 2.0f, y});
}

/******************************************************************
 * Function: drawOptions
 * ---------------------------------------------------------------
 * Renders a vertical list of options with a highlight for selection.
 ******************************************************************/
void drawOptions(sf::RenderWindow &window, const sf::Font &font,
                 const std::vector<std::string> &opts, std::size_t selected,
                 float startY) {
  float y = startY;
  for (std::size_t i = 0; i < opts.size(); ++i) {
    sf::Text t = makeText(font, opts[i], 18);
    sf::FloatRect b = t.getLocalBounds();
    if (i == selected) {
      sf::RectangleShape bg(
          sf::Vector2f(b.size.x + 24.0f, b.size.y + 14.0f));
      bg.setFillColor(sf::Color(60, 90, 160));
      bg.setOrigin({bg.getSize().x / 2.0f, bg.getSize().y / 2.0f});
      bg.setPosition({window.getSize().x / 2.0f, y + b.size.y / 2.0f});
      window.draw(bg);
    }
    centerHoriz(t, y + b.size.y / 2.0f, window.getSize().x);
    window.draw(t);
    y += 38.0f;
  }
}

struct UiState {
  Flow flow = Flow::None;
  Screen screen = Screen::MainMenu;
  std::string prompt;
  std::string message;
  std::string summary;

  // selection list
  std::vector<std::string> options;
  std::size_t selected = 0;

  // numeric selection
  bool valueMode = false;
  double value = 0.0;
  double step = 1.0;
  double minValue = 0.0;
  std::string valueLabel;

  // text input
  bool textMode = false;
  std::string textInput;

  // captured inputs
  std::string drinkType;
  std::string roastType;
  std::string coffeeStrength;
  double coffeeCups = 0.0;

  std::string latteStrength;
  std::string latteShotSize;
  int latteShots = 0;
  std::string latteMilkStyle;
  double latteMilkRatio = 0.0;
  bool needsCustomRatio = false;

  // addons
  bool isIced = false;
  std::string topping;
  int extraShots = 0;

  // presets
  std::string presetName;
  PresetManager presets;
  std::vector<Screen> history;
};

/******************************************************************
 * Function: buildCoffeeSummary
 * ---------------------------------------------------------------
 * Formats a coffee result into a multi-line summary string.
 ******************************************************************/
std::string buildCoffeeSummary(const UiState &state, const CoffeeResult &r) {
  std::string s;
  s += "Coffee Summary\n";
  s += "Roast: " + state.roastType + "\n";
  s += "Strength: " + state.coffeeStrength + " (1:" +
       std::to_string(static_cast<int>(r.ratio)) + ")\n";
  s += "Water: " + formatDouble(r.waterML) + " mL\n";
  s += "Coffee: " + formatDouble(r.coffeeGrams) + " g (" + formatDouble(r.tablespoons) + " tbsp)\n";
  if (state.isIced) s += "Addons: Iced\n";
  if (!state.topping.empty()) s += "Topping: " + state.topping;
  return s;
}

/******************************************************************
 * Function: buildLatteSummary
 * ---------------------------------------------------------------
 * Formats a latte result into a multi-line summary string.
 ******************************************************************/
std::string buildLatteSummary(const UiState &state, const LatteResult &r) {
  std::string s;
  s += "Latte Summary\n";
  s += "Strength: " + state.latteStrength + "\n";
  s += "Shots: " + std::to_string(state.latteShots) + " x " +
       state.latteShotSize + "\n";
  s += "Coffee: " + formatDouble(r.coffeeGrams) + " g (" + formatDouble(r.tablespoons) + " tbsp)\n";
  s += "Espresso: " + formatDouble(r.espressoML) + " mL\n";
  if (r.hasMilkTarget) {
    s += "Milk: " + formatDouble(r.milkML) + " mL (style " + state.latteMilkStyle + ")\n";
    s += "Final: " + formatDouble(r.finalML) + " mL\n";
  }
  if (state.isIced) s += "Addons: Iced\n";
  if (state.extraShots > 0) s += "Extra Shots: " + std::to_string(state.extraShots) + "\n";
  if (!state.topping.empty()) s += "Topping: " + state.topping;
  return s;
}

} // namespace

/******************************************************************
 * Function: runGui
 * ---------------------------------------------------------------
 * Entry point for the SFML GUI. Manages navigation, rendering, and
 * drink/preset flows using simple state tracking.
 *
 * Returns:
 *   0 on normal exit.
 ******************************************************************/
int runGui() {
  sf::RenderWindow window(sf::VideoMode({760, 540}), "Coffee & Latte Calculator",
                          sf::Style::Titlebar | sf::Style::Close);
  window.setFramerateLimit(60);

  sf::Font font;
  if (!loadFont(font))
    return 1;

  UiState state;
  bool shouldClose = false;

  std::function<void()> pushHistory = [&]() {
    if (state.screen != Screen::Summary && state.screen != Screen::Error &&
        state.screen != Screen::MainMenu) {
      state.history.push_back(state.screen);
    }
  };

  std::function<void(Screen)> configureScreen = [&](Screen s) {
    state.screen = s;
    state.valueMode = false;
    state.textMode = false;
    state.options.clear();
    state.message.clear();
    // Don't clear addons here, as we might be navigating back/forth within a flow
    // But do clear them when starting a new flow (handled in startMakeFlow)
    switch (s) {
    case Screen::MainMenu:
      state.prompt = "Select an action";
      state.options = {"Make a drink", "Create preset", "Load preset", "Quit"};
      state.selected = 0;
      break;
    case Screen::ChooseDrink:
      state.prompt = "Choose drink type";
      state.options = {"coffee", "latte"};
      state.selected = (state.drinkType == "latte") ? 1 : 0;
      break;
    case Screen::CoffeeRoast:
      state.prompt = "Select roast";
      state.options = {"light", "medium", "dark"};
      if (state.roastType == "medium")
        state.selected = 1;
      else if (state.roastType == "dark")
        state.selected = 2;
      else
        state.selected = 0;
      break;
    case Screen::CoffeeStrength:
      state.prompt = "Coffee strength";
      state.options = {"bolder", "medium", "weaker"};
      if (state.coffeeStrength == "medium")
        state.selected = 1;
      else if (state.coffeeStrength == "weaker")
        state.selected = 2;
      else
        state.selected = 0;
      break;
    case Screen::CoffeeCups:
      state.prompt = "Cups (Up/Down, Enter to confirm)";
      state.valueMode = true;
      state.valueLabel = "cups";
      state.value = (state.coffeeCups > 0.0) ? state.coffeeCups : 1.0;
      state.step = 0.5;
      state.minValue = 0.5;
      break;
    case Screen::LatteStrength:
      state.prompt = "Latte strength";
      state.options = {"stronger", "weaker"};
      state.selected = (state.latteStrength == "weaker") ? 1 : 0;
      break;
    case Screen::LatteShotSize:
      state.prompt = "Shot size";
      state.options = {"single", "double"};
      state.selected = (state.latteShotSize == "double") ? 1 : 0;
      break;
    case Screen::LatteShots:
      state.prompt = "Number of shots (Up/Down, Enter to confirm)";
      state.valueMode = true;
      state.valueLabel = "shots";
      state.value = (state.latteShots > 0) ? state.latteShots : 1;
      state.step = 1;
      state.minValue = 1;
      break;
    case Screen::LatteMilkStyle:
      state.prompt = "Milk style";
      state.options = {"none", "cortado", "flatwhite", "latte", "custom"};
      if (state.latteMilkStyle == "cortado")
        state.selected = 1;
      else if (state.latteMilkStyle == "flatwhite")
        state.selected = 2;
      else if (state.latteMilkStyle == "latte")
        state.selected = 3;
      else if (state.latteMilkStyle == "custom")
        state.selected = 4;
      else
        state.selected = 0;
      break;
    case Screen::LatteMilkRatio:
      state.prompt = "Custom milk:espresso ratio (Up/Down, Enter)";
      state.valueMode = true;
      state.valueLabel = "ratio";
      state.value = (state.latteMilkRatio >= 0.0) ? state.latteMilkRatio : 2.0;
      state.step = 0.1;
      state.minValue = 0.0;
      break;
    case Screen::PresetName:
      state.prompt = "Enter preset name (type, Enter to confirm)";
      state.textMode = true;
      state.textInput = state.presetName;
      break;
    case Screen::LoadPresetList: {
      state.prompt = "Select a preset to load";
      state.options = state.presets.getPresetNames();
      state.selected = 0;
      break;
    }
    case Screen::AddonsPrompt:
      state.prompt = "Add addons? (Ice, Toppings, etc)";
      state.options = {"No", "Yes"};
      state.selected = 0;
      break;
    case Screen::AddonsList:
      state.prompt = "Select Addon";
      if (state.drinkType == "coffee") {
        state.options = {"Ice (Cold Drink)", "Topping", "Done"};
      } else {
        state.options = {"Ice (Cold Drink)", "Topping", "Extra Shots", "Done"};
      }
      state.selected = 0;
      break;
    case Screen::ToppingInput:
      state.prompt = "Enter topping name (type, Enter)";
      state.textMode = true;
      state.textInput = state.topping;
      break;
    case Screen::ExtraShotsInput:
      state.prompt = "Extra shots (Up/Down, Enter)";
      state.valueMode = true;
      state.valueLabel = "extra";
      state.value = (state.extraShots > 0) ? state.extraShots : 1;
      state.step = 1;
      state.minValue = 1;
      break;
    case Screen::Summary:
    case Screen::Error:
      break;
    }
  };

  std::function<void(Screen, const std::string &, std::initializer_list<std::string>)>
      setOptions = [&](Screen s, const std::string &prompt,
                       std::initializer_list<std::string> opts) {
    pushHistory();
    state.screen = s;
    state.prompt = prompt;
    state.options.assign(opts);
    state.selected = 0;
    state.valueMode = false;
    state.textMode = false;
    state.textInput.clear();
    state.message.clear();
  };

  std::function<void(Screen, const std::string &, const std::vector<std::string> &)>
      setOptionsVec = [&](Screen s, const std::string &prompt,
                          const std::vector<std::string> &opts) {
    pushHistory();
    state.screen = s;
    state.prompt = prompt;
    state.options = opts;
    state.selected = 0;
    state.valueMode = false;
    state.textMode = false;
    state.textInput.clear();
    state.message.clear();
  };

  std::function<void(Screen, const std::string &, double, double, double, const std::string &)>
      setValue = [&](Screen s, const std::string &prompt, double start,
                     double step, double min, const std::string &label) {
    pushHistory();
    state.screen = s;
    state.prompt = prompt;
    state.valueMode = true;
    state.textMode = false;
    state.value = start;
    state.step = step;
    state.minValue = min;
    state.valueLabel = label;
    state.options.clear();
    state.message.clear();
  };

  std::function<void(Screen, const std::string &)> setText = [&](Screen s, const std::string &prompt) {
    pushHistory();
    state.screen = s;
    state.prompt = prompt;
    state.textMode = true;
    state.valueMode = false;
    state.textInput.clear();
    state.options.clear();
    state.message.clear();
  };

  std::function<void()> resetToMenu = [&]() {
    Flow prev = state.flow;
    state = UiState();
    state.flow = Flow::None;
    state.history.clear();
    configureScreen(Screen::MainMenu);
    if (prev == Flow::CreatePreset || prev == Flow::LoadPreset) {
      state.message = "Preset operations now available in GUI.";
    }
  };

  std::function<bool()> computeCoffee = [&]() -> bool {
    CoffeeResult r;
    if (!calcCoffee(state.coffeeStrength, state.roastType, state.coffeeCups, r))
      return false;

    if (!calcCoffee(state.coffeeStrength, state.roastType, state.coffeeCups, r))
      return false;

    applyCoffeeAddons(r, state.isIced, state.topping);

    if (state.flow == Flow::CreatePreset) {
      Presets p(state.presetName);
      p.setCoffee(state.roastType, state.coffeeStrength, state.coffeeCups);
      state.presets.addPreset(p);
      state.summary = "Preset saved: " + state.presetName + "\n\n" +
                      buildCoffeeSummary(state, r);
    } else {
      state.summary = buildCoffeeSummary(state, r);
    }
    state.screen = Screen::Summary;
    return true;
  };

  std::function<bool()> computeLatte = [&]() -> bool {
    LatteResult r;
    if (!calcLatteFromShots(state.latteStrength, state.latteShotSize,
                            state.latteShots, r))
      return false;

    applyLatteAddons(r, state.isIced, state.extraShots, state.topping);

    if (toLowerCopy(state.latteMilkStyle) != "none") {
      r.hasMilkTarget = true;
      r.milkStyle = state.latteMilkStyle;
      r.milkToEspRatio = state.latteMilkRatio;
      r.milkML = r.espressoML * r.milkToEspRatio;
      r.milkCups = r.milkML / ML_PER_CUP;
      r.finalML = r.espressoML + r.milkML;
      r.finalCups = r.finalML / ML_PER_CUP;
    }

    if (state.flow == Flow::CreatePreset) {
      Presets p(state.presetName);
      p.setLatte(state.latteShotSize, state.latteShots, state.latteStrength,
                 state.latteMilkStyle, state.latteMilkRatio);
      state.presets.addPreset(p);
      state.summary = "Preset saved: " + state.presetName + "\n\n" +
                      buildLatteSummary(state, r);
    } else {
      state.summary = buildLatteSummary(state, r);
    }
    state.screen = Screen::Summary;
    return true;
  };

  std::function<void()> startMakeFlow = [&]() {
    state.flow = Flow::Make;
    // Reset addon state
    state.isIced = false;
    state.topping.clear();
    state.extraShots = 0;
    setOptions(Screen::ChooseDrink, "Choose drink type", {"coffee", "latte"});
  };

  std::function<void()> startCreatePresetFlow = [&]() {
    state.flow = Flow::CreatePreset;
    setText(Screen::PresetName, "Enter preset name (type, Enter to confirm)");
  };

  std::function<void()> startLoadPresetFlow = [&]() {
    if (!state.presets.hasPresets()) {
      state.message = "No presets saved yet.";
      state.screen = Screen::Error;
      return;
    }
    state.flow = Flow::LoadPreset;
    std::vector<std::string> names = state.presets.getPresetNames();
    setOptionsVec(Screen::LoadPresetList, "Select a preset to load", names);
  };

  resetToMenu();

  std::function<void()> goBack = [&]() {
    if (state.history.empty())
      return;
    Screen prev = state.history.back();
    state.history.pop_back();
    configureScreen(prev);
  };

  std::function<void()> confirmSelection = [&]() {
    state.message.clear();
    switch (state.screen) {
    case Screen::MainMenu:
      if (state.selected == 0) {
        startMakeFlow();
      } else if (state.selected == 1) {
        startCreatePresetFlow();
      } else if (state.selected == 2) {
        startLoadPresetFlow();
      } else {
        shouldClose = true;
        window.close();
      }
      break;
    case Screen::ChooseDrink:
      state.drinkType = toLowerCopy(state.options[state.selected]);
      if (state.drinkType == "coffee") {
        setOptions(Screen::CoffeeRoast, "Select roast",
                   {"light", "medium", "dark"});
      } else {
        setOptions(Screen::LatteStrength, "Latte strength",
                   {"stronger", "weaker"});
      }
      break;
    case Screen::CoffeeRoast:
      state.roastType = toLowerCopy(state.options[state.selected]);
      setOptions(Screen::CoffeeStrength, "Coffee strength",
                 {"bolder", "medium", "weaker"});
      break;
    case Screen::CoffeeStrength:
      state.coffeeStrength = toLowerCopy(state.options[state.selected]);
      setValue(Screen::CoffeeCups, "Cups (Up/Down, Enter to confirm)", 1.0,
               0.5, 0.5, "cups");
      break;
    case Screen::CoffeeCups:
      state.coffeeCups = state.value;
      setOptions(Screen::AddonsPrompt, "Add addons?", {"No", "Yes"});
      break;
    case Screen::LatteStrength:
      state.latteStrength = toLowerCopy(state.options[state.selected]);
      setOptions(Screen::LatteShotSize, "Shot size", {"single", "double"});
      break;
    case Screen::LatteShotSize:
      state.latteShotSize = toLowerCopy(state.options[state.selected]);
      setValue(Screen::LatteShots, "Number of shots (Up/Down, Enter to confirm)",
               1, 1, 1, "shots");
      break;
    case Screen::LatteMilkStyle: {
      state.latteMilkStyle = toLowerCopy(state.options[state.selected]);
      double ratio = milkStyleToRatio(state.latteMilkStyle);
      if (ratio == -1.0) {
        state.needsCustomRatio = true;
        setValue(Screen::LatteMilkRatio,
                 "Custom milk:espresso ratio (Up/Down, Enter)", 2.0, 0.1, 0.0,
                 "ratio");
      } else {
        state.needsCustomRatio = false;
        state.latteMilkRatio = ratio;
        state.latteMilkRatio = ratio;
        setOptions(Screen::AddonsPrompt, "Add addons?", {"No", "Yes"});
      }
      break;
    }
    case Screen::AddonsPrompt:
      if (state.selected == 1) { // Yes
        configureScreen(Screen::AddonsList);
      } else { // No
        if (state.drinkType == "coffee") computeCoffee();
        else computeLatte();
      }
      break;
    case Screen::AddonsList: {
      std::string choice = state.options[state.selected];
      if (choice == "Done") {
        if (state.drinkType == "coffee") computeCoffee();
        else computeLatte();
      } else if (choice == "Ice (Cold Drink)") {
        state.isIced = !state.isIced; // Toggle
        state.message = state.isIced ? "Drink is now Iced" : "Drink is now Hot";
      } else if (choice == "Topping") {
        setText(Screen::ToppingInput, "Enter topping name");
      } else if (choice == "Extra Shots") {
        setValue(Screen::ExtraShotsInput, "Extra shots", 1, 1, 1, "extra");
      }
      break;
    }
    case Screen::LoadPresetList:
      if (!state.options.empty()) {
        std::string name = state.options[state.selected];
        Presets *p = state.presets.getPresetByName(name);
        if (!p) {
          state.message = "Preset not found.";
          state.screen = Screen::Error;
          break;
        }

        if (p->getDrinkType() == "coffee") {
          state.roastType = p->getRoast();
          state.coffeeStrength = p->getStrength();
          state.coffeeCups = p->getCups();
          if (!computeCoffee()) {
            state.message = "Error loading preset.";
            state.screen = Screen::Error;
          }
        } else {
          state.latteShotSize = p->getShotSize();
          state.latteShots = p->getShots();
          state.latteStrength = p->getLatteStrength();
          state.latteMilkStyle = p->getMilkStyle();
          state.latteMilkRatio = p->getMilkRatio();
          if (!computeLatte()) {
            state.message = "Error loading preset.";
            state.screen = Screen::Error;
          }
        }
      }
      break;
    case Screen::Summary:
    case Screen::Error:
      resetToMenu();
      break;
    default:
      break;
    }
  };

  std::function<void()> confirmValue = [&]() {
    switch (state.screen) {
      break;
    case Screen::CoffeeCups:
      // Handled in confirmSelection now to flow to addons
      state.coffeeCups = state.value;
      setOptions(Screen::AddonsPrompt, "Add addons?", {"No", "Yes"});
      break;
    case Screen::LatteShots:
      state.latteShots = static_cast<int>(state.value);
      setOptions(Screen::LatteMilkStyle, "Milk style",
                 {"none", "cortado", "flatwhite", "latte", "custom"});
      break;
    case Screen::LatteMilkRatio:
      state.latteMilkRatio = state.value;
      if (!computeLatte()) {
        state.message = "Calculation error.";
        state.screen = Screen::Error;
      }
      // Instead of computing immediately, go to addons
      setOptions(Screen::AddonsPrompt, "Add addons?", {"No", "Yes"});
      break;
    case Screen::ExtraShotsInput:
      state.extraShots = static_cast<int>(state.value);
      state.message = "Added " + std::to_string(state.extraShots) + " extra shots.";
      goBack(); // Return to AddonsList
      break;
    default:
      break;
    }
  };

  std::function<void()> confirmText = [&]() {
    switch (state.screen) {
    case Screen::PresetName:
      if (state.textInput.empty()) {
        state.message = "Preset name cannot be empty.";
      } else {
        state.presetName = state.textInput;
        setOptions(Screen::ChooseDrink, "Choose drink type",
                   {"coffee", "latte"});
      }
      break;
    case Screen::ToppingInput:
      state.topping = state.textInput;
      state.message = "Added topping: " + state.topping;
      goBack(); // Return to AddonsList
      break;
    default:
      break;
    }
  };

  while (window.isOpen()) {
    while (const auto event = window.pollEvent()) {
      if (event->is<sf::Event::Closed>()) {
        window.close();
      } else if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {
        if (keyPressed->code == sf::Keyboard::Key::Left ||
            keyPressed->code == sf::Keyboard::Key::Backspace) {
          goBack();
        } else if (keyPressed->code == sf::Keyboard::Key::Escape) {
          resetToMenu();
        } else if (state.valueMode) {
          if (keyPressed->code == sf::Keyboard::Key::Up) {
            state.value += state.step;
          } else if (keyPressed->code == sf::Keyboard::Key::Down) {
            state.value = std::max(state.minValue, state.value - state.step);
          } else if (keyPressed->code == sf::Keyboard::Key::Enter) {
            confirmValue();
          }
        } else if (state.textMode) {
          if (keyPressed->code == sf::Keyboard::Key::Backspace) {
            if (!state.textInput.empty())
              state.textInput.pop_back();
          } else if (keyPressed->code == sf::Keyboard::Key::Enter) {
            confirmText();
          }
        } else {
          if (keyPressed->code == sf::Keyboard::Key::Up && !state.options.empty()) {
            state.selected = (state.selected == 0) ? state.options.size() - 1
                                                   : state.selected - 1;
          } else if (keyPressed->code == sf::Keyboard::Key::Down &&
                     !state.options.empty()) {
            state.selected = (state.selected + 1) % state.options.size();
          } else if (keyPressed->code == sf::Keyboard::Key::Enter) {
            confirmSelection();
          }
        }
      } else if (const auto* textEntered = event->getIf<sf::Event::TextEntered>()) {
        if (state.textMode) {
          if (textEntered->unicode >= 32 && textEntered->unicode < 127) {
            state.textInput.push_back(static_cast<char>(textEntered->unicode));
          }
        }
      }
    }

    if (shouldClose)
      break;

    window.clear(sf::Color(18, 20, 26));

    sf::Text title = makeText(font, "Coffee & Latte Ratio Calculator", 26);
    centerHoriz(title, 40.0f, window.getSize().x);
    window.draw(title);

    sf::Text promptText = makeText(font, state.prompt, 18);
    centerHoriz(promptText, 110.0f, window.getSize().x);
    window.draw(promptText);

    if (!state.message.empty()) {
      sf::Text msg = makeText(font, state.message, 16);
      msg.setFillColor(sf::Color(255, 120, 120));
      centerHoriz(msg, 150.0f, window.getSize().x);
      window.draw(msg);
    }

    if (state.screen == Screen::Summary) {
      sf::Text sum = makeText(font, state.summary, 16);
      sum.setPosition(
          {window.getSize().x / 2.0f - sum.getLocalBounds().size.x / 2.0f, 180.0f});
      window.draw(sum);
      sf::Text hint =
          makeText(font, "Enter to return to menu, Esc to restart", 14);
      centerHoriz(hint, 330.0f, window.getSize().x);
      window.draw(hint);
    } else if (state.valueMode) {
      std::ostringstream oss;
      oss << std::fixed << std::setprecision(2) << state.value;
      std::string valueStr = state.valueLabel + ": " + oss.str();
      sf::Text v = makeText(font, valueStr, 20);
      centerHoriz(v, 200.0f, window.getSize().x);
      window.draw(v);
      sf::Text hint =
          makeText(font, "Up/Down to adjust, Enter to confirm, Esc to restart",
                   14);
      centerHoriz(hint, 240.0f, window.getSize().x);
      window.draw(hint);
    } else if (state.textMode) {
      std::string display = state.textInput.empty() ? "_" : state.textInput;
      sf::Text t = makeText(font, display, 20);
      centerHoriz(t, 200.0f, window.getSize().x);
      window.draw(t);
      sf::Text hint =
          makeText(font, "Type to edit, Enter to confirm, Esc to restart", 14);
      centerHoriz(hint, 240.0f, window.getSize().x);
      window.draw(hint);
    } else if (!state.options.empty()) {
      drawOptions(window, font, state.options, state.selected, 170.0f);
      sf::Text hint = makeText(
          font,
          "Up/Down to move, Enter to confirm, Left/backspace to go back, Esc to restart",
          14);
      centerHoriz(hint, 330.0f, window.getSize().x);
      window.draw(hint);
    }

    window.display();
  }

  return 0;
}

/******************************************************************
 * Function: main
 * ---------------------------------------------------------------
 * Program entry for the GUI build; delegates to runGui.
 ******************************************************************/
int main() { return runGui(); }
