#include "addons.hpp"

// --- Coffee Add-ons ---
// --- Coffee Add-ons ---
void applyCoffeeAddons(CoffeeResult& coffee, bool isIced, const std::string& topping) {
    if (isIced) {
        // Optional: reduce water slightly for ice
        coffee.waterML *= 0.85;  // example adjustment
        coffee.waterCups = coffee.waterML / ML_PER_CUP;
    }
    // Topping is just for display/record, doesn't change physics here
    // but we could store it if CoffeeResult had a field for it.
    // For now, GUI handles display separately.
}

void handleCoffeeAddOns(CoffeeResult& coffee) {
    char addOns;
    cout << "\nDo you want add-ons for your coffee? (y/n): ";
    cin >> addOns;
    addOns = tolower(addOns);

    if (addOns != 'y') return;

    int choice;
    cout << "\n--- Coffee Add-ons Menu ---\n";
    cout << "1) Add Ice (cold drink)\n";
    cout << "2) Toppings\n";
    cout << "Enter your choice: ";
    cin >> choice;

    bool isIced = false;
    string topping;

    switch(choice) {
        case 1:
            cout << "Turning coffee into a cold version.\n";
            isIced = true;
            break;
        case 2: {
            cout << "Enter topping type (whipped cream, cinnamon, chocolate): ";
            cin >> topping;
            cout << "Added " << topping << " to your coffee!\n";
            break;
        }
        default:
            cout << "Invalid choice.\n";
    }
    applyCoffeeAddons(coffee, isIced, topping);
}

// --- Latte Add-ons ---
void applyLatteAddons(LatteResult& latte, bool isIced, int extraShots, const std::string& topping) {
    if (extraShots > 0) {
        latte.shots += extraShots;

        // Recalculate espresso
        double gramsPerShot = (latte.shotSize == "single") ? 8.0 : 16.0;
        latte.coffeeGrams = latte.shots * gramsPerShot;
        latte.espressoML = latte.coffeeGrams * latte.brewRatio;
        latte.espressoCups = latte.espressoML / ML_PER_CUP;

        // Update milk if exists
        if (latte.hasMilkTarget) {
            latte.milkML = latte.espressoML * latte.milkToEspRatio;
            latte.milkCups = latte.milkML / ML_PER_CUP;
            latte.finalML = latte.espressoML + latte.milkML;
            latte.finalCups = latte.finalML / ML_PER_CUP;
        }
    }

    if (isIced) {
        // Optional: reduce espresso/milk ratio for cold drink
        latte.brewRatio *= 0.8;
        latte.espressoML = latte.coffeeGrams * latte.brewRatio;
        latte.espressoCups = latte.espressoML / ML_PER_CUP;

        if (latte.hasMilkTarget) {
            latte.milkML = latte.espressoML * latte.milkToEspRatio;
            latte.milkCups = latte.milkML / ML_PER_CUP;
            latte.finalML = latte.espressoML + latte.milkML;
            latte.finalCups = latte.finalML / ML_PER_CUP;
        }
    }
}

void handleLatteAddOns(LatteResult& latte) {
    char addOns;
    cout << "\nDo you want add-ons for your latte? (y/n): ";
    cin >> addOns;
    addOns = tolower(addOns);

    if (addOns != 'y') return;

    int choice;
    cout << "\n--- Latte Add-ons Menu ---\n";
    cout << "1) Extra Espresso Shots\n";
    cout << "2) Add Ice (cold latte)\n";
    cout << "3) Toppings\n";
    cout << "Enter your choice: ";
    cin >> choice;

    bool isIced = false;
    int extraShots = 0;
    string topping;

    switch(choice) {
        case 1: {
            cout << "How many extra shots? ";
            cin >> extraShots;
            break;
        }
        case 2:
            cout << "Turning latte into a cold version.\n";
            isIced = true;
            break;
        case 3: {
            cout << "Enter topping type (whipped cream, cinnamon, chocolate): ";
            cin >> topping;
            cout << "Added " << topping << " to your latte!\n";
            break;
        }
        default:
            cout << "Invalid choice.\n";
    }
    applyLatteAddons(latte, isIced, extraShots, topping);
}
