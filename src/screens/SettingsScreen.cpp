#include "screens/SettingsScreen.h"
#include <Fonts/FreeSans9pt7b.h>

static const char* modeName(ClockNightMode m) {
    switch (m) {
        case ClockNightMode::DAY: return "DAY";
        case ClockNightMode::NIGHT: return "NIGHT";
        case ClockNightMode::AUTO: return "AUTO";
        default: return "?";
    }
}

void SettingsScreen::begin() {
    draw();
}

void SettingsScreen::onOk() {
    // Цикл режима
    if (clockNightMode == ClockNightMode::DAY) clockNightMode = ClockNightMode::NIGHT;
    else if (clockNightMode == ClockNightMode::NIGHT) clockNightMode = ClockNightMode::AUTO;
    else clockNightMode = ClockNightMode::DAY;

    draw();
}

void SettingsScreen::onBack() {
    if (_clock) _sm.set(_clock);
}

void SettingsScreen::draw() {
    _tft.fillScreen(ST77XX_BLACK);
    _tft.setFont(&FreeSans9pt7b);
    _tft.setTextColor(ST77XX_WHITE, ST77XX_BLACK);

    _tft.setCursor(10, 30);
    _tft.print("SETTINGS");

    _tft.setCursor(10, 60);
    _tft.print("Night mode: ");
    _tft.print(modeName(clockNightMode));

    _tft.setCursor(10, 90);
    _tft.print("OK = change");
}
