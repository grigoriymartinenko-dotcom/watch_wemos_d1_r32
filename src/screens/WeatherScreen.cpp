#include "screens/WeatherScreen.h"
#include <Fonts/FreeSans9pt7b.h>

void WeatherScreen::begin() {
    _tft.fillScreen(ST77XX_BLACK);
    _tft.setFont(&FreeSans9pt7b);
    _tft.setTextColor(ST77XX_WHITE, ST77XX_BLACK);
    _tft.setCursor(10, 30);
    _tft.print("WEATHER");

    _tft.setCursor(10, 55);
    _tft.print("(baseline placeholder)");
}

void WeatherScreen::onBack() {
    if (_clock) _sm.set(_clock);
}
