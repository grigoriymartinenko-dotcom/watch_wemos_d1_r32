#include <Arduino.h>
#include "screens/ForecastScreen.h"
#include "Config.h"

ForecastScreen::ForecastScreen(
    Adafruit_ST7735& tft,
    ForecastService& forecast,
    ScreenManager& manager,
    Screen* backScreen
)
: _tft(tft)
, _forecast(forecast)
, _manager(manager)
, _back(backScreen)
{}

void ForecastScreen::begin() {
    _redraw = true;
}

void ForecastScreen::update() {
    if (!_redraw) return;

    _tft.fillScreen(ST77XX_BLACK);
    drawHeader();
    drawContent();

    _redraw = false;
}

void ForecastScreen::onBack() {
    if (_back) {
        _manager.set(_back);
    }
}

void ForecastScreen::drawHeader() {
    _tft.setTextSize(1);
    _tft.setTextColor(ST77XX_WHITE);
    _tft.setCursor(2, 2);
    _tft.print("FORECAST ");
    _tft.print(CITY_NAME);
}

void ForecastScreen::drawContent() {
    _tft.setCursor(10, 40);

    if (!_forecast.data().valid) {
        _tft.setTextColor(ST77XX_RED);
        _tft.print("NO DATA");
        return;
    }

    _tft.setTextColor(ST77XX_WHITE);
    _tft.print("DATA OK");
}