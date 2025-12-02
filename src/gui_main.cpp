#include "gui.hpp"
#include "constants.hpp"
#include "functions.hpp"
#include "presets.hpp"
#include "presetsManager.hpp"
#include <SFML/Graphics.hpp>
#include <algorithm>
#include <string>
#include <vector>

namespace {

// Screen codes (kept as ints to avoid enum usage)
constexpr int SCREEN_MAIN = 0;
constexpr int SCREEN_CHOOSE_DRINK = 1;
constexpr int SCREEN_COFFEE_ROAST = 2;
constexpr int SCREEN_COFFEE_STRENGTH = 3;
constexpr int SCREEN_COFFEE_CUPS = 4;
constexpr int SCREEN_LATTE_STRENGTH = 5;
constexpr int SCREEN_LATTE_SHOT_SIZE = 6;
constexpr int SCREEN_LATTE_SHOTS = 7;
constexpr int SCREEN_LATTE_MILK_STYLE = 8;
constexpr int SCREEN_LATTE_MILK_RATIO = 9;
constexpr int SCREEN_PRESET_NAME = 10;
constexpr int SCREEN_LOAD_PRESET_LIST = 11;
constexpr int SCREEN_SUMMARY = 12;
constexpr int SCREEN_ERROR = 13;

// Flow codes
constexpr int FLOW_NONE = 0;
constexpr int FLOW_MAKE = 1;
constexpr int FLOW_CREATE_PRESET = 2;
constexpr int FLOW_LOAD_PRESET = 3;

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
      sf::RectangleShape bg(sf::Vector2f(b.width + 24.0f, b.height + 14.0f));
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

std::string buildCoffeeSummary(const std::string &roast,
                               const std::string &strength,
                               const CoffeeResult &r) {
  std::string s;
  s += "Coffee Summary\n";
  s += "Roast: " + roast + "\n";
  s += "Strength: " + strength + " (1:" +
       std::to_string(static_cast<int>(r.ratio)) + ")\n";
  s += "Water: " + std::to_string(r.waterML) + " mL\n";
  s += "Coffee: " + std::to_string(r.coffeeGrams) + " g (" +
       std::to_string(r.tablespoons) + " tbsp)";
  return s;
}

std::string buildLatteSummary(const std::string &strength,
                              const std::string &shotSize, int shots,
                              const std::string &milkStyle, double milkRatio,
                              const LatteResult &r) {
  std::string s;
  s += "Latte Summary\n";
  s += "Strength: " + strength + "\n";
  s += "Shots: " + std::to_string(shots) + " x " + shotSize + "\n";
  s += "Coffee: " + std::to_string(r.coffeeGrams) + " g (" +
       std::to_string(r.tablespoons) + " tbsp)\n";
  s += "Espresso: " + std::to_string(r.espressoML) + " mL\n";
  if (r.hasMilkTarget) {
    s += "Milk: " + std::to_string(r.milkML) + " mL (style " + milkStyle + ")";
    s += "\nFinal: " + std::to_string(r.finalML) + " mL";
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

  int flow = FLOW_NONE;
  int screen = SCREEN_MAIN;
  std::string prompt;
  std::string message;
  std::string summary;

  std::vector<std::string> options;
  std::size_t selected = 0;

  bool valueMode = false;
  double value = 0.0;
  double step = 1.0;
  double minValue = 0.0;
  std::string valueLabel;

  bool textMode = false;
  std::string textInput;

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

  std::string presetName;
  PresetManager presets;
  std::vector<int> history;
  bool shouldClose = false;

  auto pushHistory = [&]() {
    if (screen != SCREEN_MAIN && screen != SCREEN_SUMMARY &&
        screen != SCREEN_ERROR) {
      history.push_back(screen);
    }
  };

  auto configureScreen = [&](int s) {
    screen = s;
    valueMode = false;
    textMode = false;
    options.clear();
    message.clear();

    if (s == SCREEN_MAIN) {
      prompt = "Select an action";
      options = {"Make a drink", "Create preset", "Load preset", "Quit"};
      selected = 0;
    } else if (s == SCREEN_CHOOSE_DRINK) {
      prompt = "Choose drink type";
      options = {"coffee", "latte"};
      selected = (drinkType == "latte") ? 1 : 0;
    } else if (s == SCREEN_COFFEE_ROAST) {
      prompt = "Select roast";
      options = {"light", "medium", "dark"};
      if (roastType == "medium")
        selected = 1;
      else if (roastType == "dark")
        selected = 2;
      else
        selected = 0;
    } else if (s == SCREEN_COFFEE_STRENGTH) {
      prompt = "Coffee strength";
      options = {"bolder", "medium", "weaker"};
      if (coffeeStrength == "medium")
        selected = 1;
      else if (coffeeStrength == "weaker")
        selected = 2;
      else
        selected = 0;
    } else if (s == SCREEN_COFFEE_CUPS) {
      prompt = "Cups (Up/Down, Enter to confirm)";
      valueMode = true;
      valueLabel = "cups";
      value = (coffeeCups > 0.0) ? coffeeCups : 1.0;
      step = 0.5;
      minValue = 0.5;
    } else if (s == SCREEN_LATTE_STRENGTH) {
      prompt = "Latte strength";
      options = {"stronger", "weaker"};
      selected = (latteStrength == "weaker") ? 1 : 0;
    } else if (s == SCREEN_LATTE_SHOT_SIZE) {
      prompt = "Shot size";
      options = {"single", "double"};
      selected = (latteShotSize == "double") ? 1 : 0;
    } else if (s == SCREEN_LATTE_SHOTS) {
      prompt = "Number of shots (Up/Down, Enter to confirm)";
      valueMode = true;
      valueLabel = "shots";
      value = (latteShots > 0) ? latteShots : 1;
      step = 1;
      minValue = 1;
    } else if (s == SCREEN_LATTE_MILK_STYLE) {
      prompt = "Milk style";
      options = {"none", "cortado", "flatwhite", "latte", "custom"};
      if (latteMilkStyle == "cortado")
        selected = 1;
      else if (latteMilkStyle == "flatwhite")
        selected = 2;
      else if (latteMilkStyle == "latte")
        selected = 3;
      else if (latteMilkStyle == "custom")
        selected = 4;
      else
        selected = 0;
    } else if (s == SCREEN_LATTE_MILK_RATIO) {
      prompt = "Custom milk:espresso ratio (Up/Down, Enter)";
      valueMode = true;
      valueLabel = "ratio";
      value = (latteMilkRatio >= 0.0) ? latteMilkRatio : 2.0;
      step = 0.1;
      minValue = 0.0;
    } else if (s == SCREEN_PRESET_NAME) {
      prompt = "Enter preset name (type, Enter to confirm)";
      textMode = true;
      textInput = presetName;
    } else if (s == SCREEN_LOAD_PRESET_LIST) {
      prompt = "Select a preset to load";
      options = presets.getPresetNames();
      selected = 0;
    }
  };

  auto setOptions = [&](int s, const std::string &p,
                        std::initializer_list<std::string> opts) {
    pushHistory();
    screen = s;
    prompt = p;
    options.assign(opts);
    selected = 0;
    valueMode = false;
    textMode = false;
    textInput.clear();
    message.clear();
  };

  auto setOptionsVec = [&](int s, const std::string &p,
                           const std::vector<std::string> &opts) {
    pushHistory();
    screen = s;
    prompt = p;
    options = opts;
    selected = 0;
    valueMode = false;
    textMode = false;
    textInput.clear();
    message.clear();
  };

  auto setValue = [&](int s, const std::string &p, double start, double stepIn,
                      double min, const std::string &label) {
    pushHistory();
    screen = s;
    prompt = p;
    valueMode = true;
    textMode = false;
    value = start;
    step = stepIn;
    minValue = min;
    valueLabel = label;
    options.clear();
    message.clear();
  };

  auto setText = [&](int s, const std::string &p) {
    pushHistory();
    screen = s;
    prompt = p;
    textMode = true;
    valueMode = false;
    textInput.clear();
    options.clear();
    message.clear();
  };

  auto resetToMenu = [&]() {
    int prev = flow;
    flow = FLOW_NONE;
    screen = SCREEN_MAIN;
    prompt = "Select an action";
    options = {"Make a drink", "Create preset", "Load preset", "Quit"};
    selected = 0;
    valueMode = false;
    textMode = false;
    textInput.clear();
    message.clear();
    summary.clear();
    drinkType.clear();
    roastType.clear();
    coffeeStrength.clear();
    coffeeCups = 0.0;
    latteStrength.clear();
    latteShotSize.clear();
    latteShots = 0;
    latteMilkStyle.clear();
    latteMilkRatio = 0.0;
    needsCustomRatio = false;
    presetName.clear();
    history.clear();
    shouldClose = false;
    if (prev == FLOW_CREATE_PRESET || prev == FLOW_LOAD_PRESET) {
      message = "Preset operations now available in GUI.";
    }
  };

  resetToMenu();

  auto computeCoffee = [&]() -> bool {
    CoffeeResult r;
    if (!calcCoffee(coffeeStrength, roastType, coffeeCups, r))
      return false;

    if (flow == FLOW_CREATE_PRESET) {
      Presets p(presetName);
      p.setCoffee(roastType, coffeeStrength, coffeeCups);
      presets.addPreset(p);
      summary = "Preset saved: " + presetName + "\n\n" +
                buildCoffeeSummary(roastType, coffeeStrength, r);
    } else {
      summary = buildCoffeeSummary(roastType, coffeeStrength, r);
    }
    screen = SCREEN_SUMMARY;
    return true;
  };

  auto computeLatte = [&]() -> bool {
    LatteResult r;
    if (!calcLatteFromShots(latteStrength, latteShotSize, latteShots, r))
      return false;

    if (toLowerCopy(latteMilkStyle) != "none") {
      r.hasMilkTarget = true;
      r.milkStyle = latteMilkStyle;
      r.milkToEspRatio = latteMilkRatio;
      r.milkML = r.espressoML * r.milkToEspRatio;
      r.milkCups = r.milkML / ML_PER_CUP;
      r.finalML = r.espressoML + r.milkML;
      r.finalCups = r.finalML / ML_PER_CUP;
    }

    if (flow == FLOW_CREATE_PRESET) {
      Presets p(presetName);
      p.setLatte(latteShotSize, latteShots, latteStrength, latteMilkStyle,
                 latteMilkRatio);
      presets.addPreset(p);
      summary = "Preset saved: " + presetName + "\n\n" +
                buildLatteSummary(latteStrength, latteShotSize, latteShots,
                                  latteMilkStyle, latteMilkRatio, r);
    } else {
      summary = buildLatteSummary(latteStrength, latteShotSize, latteShots,
                                  latteMilkStyle, latteMilkRatio, r);
    }
    screen = SCREEN_SUMMARY;
    return true;
  };

  auto startMakeFlow = [&]() {
    flow = FLOW_MAKE;
    setOptions(SCREEN_CHOOSE_DRINK, "Choose drink type", {"coffee", "latte"});
  };

  auto startCreatePresetFlow = [&]() {
    flow = FLOW_CREATE_PRESET;
    setText(SCREEN_PRESET_NAME, "Enter preset name (type, Enter to confirm)");
  };

  auto startLoadPresetFlow = [&]() {
    if (!presets.hasPresets()) {
      message = "No presets saved yet.";
      screen = SCREEN_ERROR;
      return;
    }
    flow = FLOW_LOAD_PRESET;
    auto names = presets.getPresetNames();
    setOptionsVec(SCREEN_LOAD_PRESET_LIST, "Select a preset to load", names);
  };

  auto goBack = [&]() {
    if (history.empty())
      return;
    int prev = history.back();
    history.pop_back();
    configureScreen(prev);
  };

  auto confirmSelection = [&]() {
    message.clear();
    if (screen == SCREEN_MAIN) {
      if (selected == 0) {
        startMakeFlow();
      } else if (selected == 1) {
        startCreatePresetFlow();
      } else if (selected == 2) {
        startLoadPresetFlow();
      } else {
        shouldClose = true;
        window.close();
      }
    } else if (screen == SCREEN_CHOOSE_DRINK) {
      drinkType = toLowerCopy(options[selected]);
      if (drinkType == "coffee") {
        setOptions(SCREEN_COFFEE_ROAST, "Select roast",
                   {"light", "medium", "dark"});
      } else {
        setOptions(SCREEN_LATTE_STRENGTH, "Latte strength",
                   {"stronger", "weaker"});
      }
    } else if (screen == SCREEN_COFFEE_ROAST) {
      roastType = toLowerCopy(options[selected]);
      setOptions(SCREEN_COFFEE_STRENGTH, "Coffee strength",
                 {"bolder", "medium", "weaker"});
    } else if (screen == SCREEN_COFFEE_STRENGTH) {
      coffeeStrength = toLowerCopy(options[selected]);
      setValue(SCREEN_COFFEE_CUPS, "Cups (Up/Down, Enter to confirm)", 1.0,
               0.5, 0.5, "cups");
    } else if (screen == SCREEN_LATTE_STRENGTH) {
      latteStrength = toLowerCopy(options[selected]);
      setOptions(SCREEN_LATTE_SHOT_SIZE, "Shot size", {"single", "double"});
    } else if (screen == SCREEN_LATTE_SHOT_SIZE) {
      latteShotSize = toLowerCopy(options[selected]);
      setValue(SCREEN_LATTE_SHOTS, "Number of shots (Up/Down, Enter to confirm)",
               1, 1, 1, "shots");
    } else if (screen == SCREEN_LATTE_MILK_STYLE) {
      latteMilkStyle = toLowerCopy(options[selected]);
      double ratio = milkStyleToRatio(latteMilkStyle);
      if (ratio == -1.0) {
        needsCustomRatio = true;
        setValue(SCREEN_LATTE_MILK_RATIO,
                 "Custom milk:espresso ratio (Up/Down, Enter)", 2.0, 0.1, 0.0,
                 "ratio");
      } else {
        needsCustomRatio = false;
        latteMilkRatio = ratio;
        computeLatte();
      }
    } else if (screen == SCREEN_LOAD_PRESET_LIST) {
      if (!options.empty()) {
        std::string name = options[selected];
        Presets *p = presets.getPresetByName(name);
        if (!p) {
          message = "Preset not found.";
          screen = SCREEN_ERROR;
        } else if (p->getDrinkType() == "coffee") {
          roastType = p->getRoast();
          coffeeStrength = p->getStrength();
          coffeeCups = p->getCups();
          if (!computeCoffee()) {
            message = "Error loading preset.";
            screen = SCREEN_ERROR;
          }
        } else {
          latteShotSize = p->getShotSize();
          latteShots = p->getShots();
          latteStrength = p->getLatteStrength();
          latteMilkStyle = p->getMilkStyle();
          latteMilkRatio = p->getMilkRatio();
          if (!computeLatte()) {
            message = "Error loading preset.";
            screen = SCREEN_ERROR;
          }
        }
      }
    } else if (screen == SCREEN_SUMMARY || screen == SCREEN_ERROR) {
      resetToMenu();
    }
  };

  auto confirmValue = [&]() {
    if (screen == SCREEN_COFFEE_CUPS) {
      coffeeCups = value;
      if (!computeCoffee()) {
        message = "Calculation error.";
        screen = SCREEN_ERROR;
      }
    } else if (screen == SCREEN_LATTE_SHOTS) {
      latteShots = static_cast<int>(value);
      setOptions(SCREEN_LATTE_MILK_STYLE, "Milk style",
                 {"none", "cortado", "flatwhite", "latte", "custom"});
    } else if (screen == SCREEN_LATTE_MILK_RATIO) {
      latteMilkRatio = value;
      if (!computeLatte()) {
        message = "Calculation error.";
        screen = SCREEN_ERROR;
      }
    }
  };

  auto confirmText = [&]() {
    if (screen == SCREEN_PRESET_NAME) {
      if (textInput.empty()) {
        message = "Preset name cannot be empty.";
      } else {
        presetName = textInput;
        setOptions(SCREEN_CHOOSE_DRINK, "Choose drink type",
                   {"coffee", "latte"});
      }
    }
  };

  while (window.isOpen()) {
    sf::Event event{};
    while (window.pollEvent(event)) {
      if (event.type == sf::Event::Closed) {
        window.close();
      } else if (event.type == sf::Event::KeyPressed) {
        if (event.key.code == sf::Keyboard::Left) {
          goBack();
        } else if (event.key.code == sf::Keyboard::Escape) {
          resetToMenu();
        } else if (valueMode) {
          if (event.key.code == sf::Keyboard::Up) {
            value += step;
          } else if (event.key.code == sf::Keyboard::Down) {
            value = std::max(minValue, value - step);
          } else if (event.key.code == sf::Keyboard::Enter ||
                     event.key.code == sf::Keyboard::Return) {
            confirmValue();
          }
        } else if (textMode) {
          if (event.key.code == sf::Keyboard::BackSpace) {
            if (!textInput.empty())
              textInput.pop_back();
          } else if (event.key.code == sf::Keyboard::Enter ||
                     event.key.code == sf::Keyboard::Return) {
            confirmText();
          }
        } else {
          if (event.key.code == sf::Keyboard::Up && !options.empty()) {
            selected = (selected == 0) ? options.size() - 1 : selected - 1;
          } else if (event.key.code == sf::Keyboard::Down &&
                     !options.empty()) {
            selected = (selected + 1) % options.size();
          } else if (event.key.code == sf::Keyboard::Enter ||
                     event.key.code == sf::Keyboard::Return) {
            confirmSelection();
          }
        }
      } else if (event.type == sf::Event::TextEntered && textMode) {
        if (event.text.unicode >= 32 && event.text.unicode < 127) {
          textInput.push_back(static_cast<char>(event.text.unicode));
        }
      }
    }

    if (shouldClose)
      break;

    window.clear(sf::Color(18, 20, 26));

    sf::Text title = makeText(font, "Coffee & Latte Ratio Calculator", 26);
    centerHoriz(title, 40.0f, window.getSize().x);
    window.draw(title);

    sf::Text promptText = makeText(font, prompt, 18);
    centerHoriz(promptText, 110.0f, window.getSize().x);
    window.draw(promptText);

    if (!message.empty()) {
      sf::Text msg = makeText(font, message, 16);
      msg.setFillColor(sf::Color(255, 120, 120));
      centerHoriz(msg, 150.0f, window.getSize().x);
      window.draw(msg);
    }

    if (screen == SCREEN_SUMMARY) {
      sf::Text sum = makeText(font, summary, 16);
      sum.setPosition(
          window.getSize().x / 2.0f - sum.getLocalBounds().width / 2.0f,
          180.0f);
      window.draw(sum);
      sf::Text hint =
          makeText(font, "Enter to return to menu, Esc to restart", 14);
      centerHoriz(hint, 330.0f, window.getSize().x);
      window.draw(hint);
    } else if (valueMode) {
      std::string valueStr = valueLabel + ": " + std::to_string(value);
      sf::Text v = makeText(font, valueStr, 20);
      centerHoriz(v, 200.0f, window.getSize().x);
      window.draw(v);
      sf::Text hint =
          makeText(font, "Up/Down to adjust, Enter to confirm, Esc to restart",
                   14);
      centerHoriz(hint, 240.0f, window.getSize().x);
      window.draw(hint);
    } else if (textMode) {
      std::string display = textInput.empty() ? "_" : textInput;
      sf::Text t = makeText(font, display, 20);
      centerHoriz(t, 200.0f, window.getSize().x);
      window.draw(t);
      sf::Text hint =
          makeText(font, "Type to edit, Enter to confirm, Esc to restart", 14);
      centerHoriz(hint, 240.0f, window.getSize().x);
      window.draw(hint);
    } else if (!options.empty()) {
      drawOptions(window, font, options, selected, 170.0f);
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
