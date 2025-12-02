#include "gui.hpp"
#include "constants.hpp"
#include "functions.hpp"
#include "presets.hpp"
#include "presetsManager.hpp"
#include <SFML/Graphics.hpp>
#include <string>
#include <vector>

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
  Summary,
  Error
};

bool loadFont(sf::Font &font) {
  const std::vector<std::string> candidates = {
      "resources/DejaVuSans.ttf",
      "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf",
      "/usr/local/share/fonts/DejaVuSans.ttf",
      "C:\\\\Windows\\\\Fonts\\\\arial.ttf"};
  for (const auto &path : candidates) {
    if (font.loadFromFile(path))
      return true;
  }
  return false;
}

sf::Text makeText(const sf::Font &font, const std::string &str, unsigned size) {
  sf::Text t(str, font, size);
  t.setFillColor(sf::Color(240, 240, 240));
  return t;
}

void centerHoriz(sf::Text &text, float y, float width) {
  sf::FloatRect b = text.getLocalBounds();
  text.setOrigin(b.left + b.width / 2.0f, b.top + b.height / 2.0f);
  text.setPosition(width / 2.0f, y);
}

void drawOptions(sf::RenderWindow &window, const sf::Font &font,
                 const std::vector<std::string> &opts, std::size_t selected,
                 float startY) {
  float y = startY;
  for (std::size_t i = 0; i < opts.size(); ++i) {
    sf::Text t = makeText(font, opts[i], 18);
    sf::FloatRect b = t.getLocalBounds();
    if (i == selected) {
      sf::RectangleShape bg(
          sf::Vector2f(b.width + 24.0f, b.height + 14.0f));
      bg.setFillColor(sf::Color(60, 90, 160));
      bg.setOrigin(bg.getSize().x / 2.0f, bg.getSize().y / 2.0f);
      bg.setPosition(window.getSize().x / 2.0f, y + b.height / 2.0f);
      window.draw(bg);
    }
    centerHoriz(t, y + b.height / 2.0f, window.getSize().x);
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

  // presets
  std::string presetName;
  PresetManager presets;
};

std::string buildCoffeeSummary(const UiState &state, const CoffeeResult &r) {
  std::string s;
  s += "Coffee Summary\n";
  s += "Roast: " + state.roastType + "\n";
  s += "Strength: " + state.coffeeStrength + " (1:" +
       std::to_string(static_cast<int>(r.ratio)) + ")\n";
  s += "Water: " + std::to_string(r.waterML) + " mL\n";
  s += "Coffee: " + std::to_string(r.coffeeGrams) + " g (" +
       std::to_string(r.tablespoons) + " tbsp)";
  return s;
}

std::string buildLatteSummary(const UiState &state, const LatteResult &r) {
  std::string s;
  s += "Latte Summary\n";
  s += "Strength: " + state.latteStrength + "\n";
  s += "Shots: " + std::to_string(state.latteShots) + " x " +
       state.latteShotSize + "\n";
  s += "Coffee: " + std::to_string(r.coffeeGrams) + " g (" +
       std::to_string(r.tablespoons) + " tbsp)\n";
  s += "Espresso: " + std::to_string(r.espressoML) + " mL\n";
  if (r.hasMilkTarget) {
    s += "Milk: " + std::to_string(r.milkML) + " mL (style " +
         state.latteMilkStyle + ")\n";
    s += "Final: " + std::to_string(r.finalML) + " mL";
  }
  return s;
}

} // namespace

int runGui() {
  sf::RenderWindow window(sf::VideoMode(760, 540), "Coffee & Latte Calculator",
                          sf::Style::Titlebar | sf::Style::Close);
  window.setFramerateLimit(60);

  sf::Font font;
  if (!loadFont(font))
    return 1;

  UiState state;

  auto setOptions = [&](Screen s, const std::string &prompt,
                        std::initializer_list<std::string> opts) {
    state.screen = s;
    state.prompt = prompt;
    state.options.assign(opts);
    state.selected = 0;
    state.valueMode = false;
    state.textMode = false;
    state.textInput.clear();
    state.message.clear();
  };

  auto setOptionsVec = [&](Screen s, const std::string &prompt,
                           const std::vector<std::string> &opts) {
    state.screen = s;
    state.prompt = prompt;
    state.options = opts;
    state.selected = 0;
    state.valueMode = false;
    state.textMode = false;
    state.textInput.clear();
    state.message.clear();
  };

  auto setValue = [&](Screen s, const std::string &prompt, double start,
                      double step, double min, const std::string &label) {
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

  auto setText = [&](Screen s, const std::string &prompt) {
    state.screen = s;
    state.prompt = prompt;
    state.textMode = true;
    state.valueMode = false;
    state.textInput.clear();
    state.options.clear();
    state.message.clear();
  };

  auto resetToMenu = [&]() {
    Flow prev = state.flow;
    state = UiState();
    state.flow = Flow::None;
    setOptions(Screen::MainMenu, "Select an action",
               {"Make a drink", "Create preset", "Load preset", "Quit"});
    if (prev == Flow::CreatePreset || prev == Flow::LoadPreset) {
      state.message = "Preset operations now available in GUI.";
    }
  };

  auto computeCoffee = [&]() -> bool {
    CoffeeResult r;
    if (!calcCoffee(state.coffeeStrength, state.roastType, state.coffeeCups, r))
      return false;

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

  auto computeLatte = [&]() -> bool {
    LatteResult r;
    if (!calcLatteFromShots(state.latteStrength, state.latteShotSize,
                            state.latteShots, r))
      return false;

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

  auto startMakeFlow = [&]() {
    state.flow = Flow::Make;
    setOptions(Screen::ChooseDrink, "Choose drink type", {"coffee", "latte"});
  };

  auto startCreatePresetFlow = [&]() {
    state.flow = Flow::CreatePreset;
    setText(Screen::PresetName, "Enter preset name (type, Enter to confirm)");
  };

  auto startLoadPresetFlow = [&]() {
    if (!state.presets.hasPresets()) {
      state.message = "No presets saved yet.";
      state.screen = Screen::Error;
      return;
    }
    state.flow = Flow::LoadPreset;
    auto names = state.presets.getPresetNames();
    setOptionsVec(Screen::LoadPresetList, "Select a preset to load", names);
  };

  resetToMenu();

  auto confirmSelection = [&]() {
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
        // Quit
        state.summary = "Goodbye!";
        state.screen = Screen::Summary;
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
        computeLatte();
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

  auto confirmValue = [&]() {
    switch (state.screen) {
    case Screen::CoffeeCups:
      state.coffeeCups = state.value;
      if (!computeCoffee()) {
        state.message = "Calculation error.";
        state.screen = Screen::Error;
      }
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
      break;
    default:
      break;
    }
  };

  auto confirmText = [&]() {
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
    default:
      break;
    }
  };

  while (window.isOpen()) {
    sf::Event event{};
    while (window.pollEvent(event)) {
      if (event.type == sf::Event::Closed) {
        window.close();
      } else if (event.type == sf::Event::KeyPressed) {
        if (event.key.code == sf::Keyboard::Escape) {
          resetToMenu();
        } else if (state.valueMode) {
          if (event.key.code == sf::Keyboard::Up) {
            state.value += state.step;
          } else if (event.key.code == sf::Keyboard::Down) {
            state.value = std::max(state.minValue, state.value - state.step);
          } else if (event.key.code == sf::Keyboard::Enter ||
                     event.key.code == sf::Keyboard::Return) {
            confirmValue();
          }
        } else if (state.textMode) {
          if (event.key.code == sf::Keyboard::BackSpace) {
            if (!state.textInput.empty())
              state.textInput.pop_back();
          } else if (event.key.code == sf::Keyboard::Enter ||
                     event.key.code == sf::Keyboard::Return) {
            confirmText();
          }
        } else {
          if (event.key.code == sf::Keyboard::Up && !state.options.empty()) {
            state.selected = (state.selected == 0) ? state.options.size() - 1
                                                   : state.selected - 1;
          } else if (event.key.code == sf::Keyboard::Down &&
                     !state.options.empty()) {
            state.selected = (state.selected + 1) % state.options.size();
          } else if (event.key.code == sf::Keyboard::Enter ||
                     event.key.code == sf::Keyboard::Return) {
            confirmSelection();
          }
        }
      } else if (event.type == sf::Event::TextEntered && state.textMode) {
        if (event.text.unicode >= 32 && event.text.unicode < 127) {
          state.textInput.push_back(static_cast<char>(event.text.unicode));
        }
      }
    }

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
          window.getSize().x / 2.0f - sum.getLocalBounds().width / 2.0f, 180.0f);
      window.draw(sum);
      sf::Text hint =
          makeText(font, "Enter to return to menu, Esc to restart", 14);
      centerHoriz(hint, 330.0f, window.getSize().x);
      window.draw(hint);
    } else if (state.valueMode) {
      std::string valueStr =
          state.valueLabel + ": " + std::to_string(state.value);
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
      sf::Text hint =
          makeText(font, "Up/Down to move, Enter to confirm, Esc to restart",
                   14);
      centerHoriz(hint, 330.0f, window.getSize().x);
      window.draw(hint);
    }

    window.display();
  }

  return 0;
}

int main() { return runGui(); }
