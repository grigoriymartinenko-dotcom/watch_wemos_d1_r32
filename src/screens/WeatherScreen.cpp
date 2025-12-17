#include "screens/WeatherScreen.h"
#include "ui/Theme.h"

WeatherScreen::WeatherScreen(Adafruit_ST7735& tft, ScreenManager& sm)
    : _tft(tft), _sm(sm) {}

void WeatherScreen::setClock(Screen* clock) {
    _clock = clock;
}

void WeatherScreen::begin() {
    _tft.fillScreen(C_UI_BG_NIGHT);
    draw();
}

void WeatherScreen::update() {
    // позже: обновление погоды
}

void WeatherScreen::onUp() {
    // зарезервировано (прогноз вверх/вниз)
}

void WeatherScreen::onDown() {
    // зарезервировано
}

void WeatherScreen::onOk() {
    // позже: обновить вручную
}

void WeatherScreen::onBack() {
    if (_clock) {
        _sm.set(_clock);
    }
}

void WeatherScreen::draw() {
    _tft.setTextColor(ST77XX_WHITE);
    _tft.setCursor(10, 20);
    _tft.print("Weather");

    _tft.setCursor(10, 50);
    _tft.print("Loading...");
}