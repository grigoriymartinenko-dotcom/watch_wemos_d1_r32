#include "screens/WeatherScreen.h"
#include "ui/Theme.h"
#include "services/WeatherService.h"
#include <WiFi.h>

extern WeatherService weather;

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
    if (weather.update()) {
        draw();
    }
}

void WeatherScreen::onUp() {}
void WeatherScreen::onDown() {}

void WeatherScreen::onOk() {
    weather.update();
    draw();
}

void WeatherScreen::onBack() {
    if (_clock) {
        _sm.set(_clock);
    }
}

void WeatherScreen::draw() {
    uint16_t bg = C_UI_BG_NIGHT;
    _tft.fillRect(0, 0, _tft.width(), _tft.height(), bg);
    _tft.setTextColor(ST77XX_WHITE, bg);

    /* ===== HEADER ===== */
    _tft.setCursor(4, 4);
    _tft.print("Weather");

    _tft.setCursor(_tft.width() - 40, 4);
    _tft.setTextColor(WiFi.isConnected() ? ST77XX_GREEN : ST77XX_RED, bg);
    _tft.print(WiFi.isConnected() ? "WiFi" : "NoWi");
    _tft.setTextColor(ST77XX_WHITE, bg);

    _tft.drawFastHLine(0, 18, _tft.width(), C_GRAY_40);

    /* ===== CONTENT ===== */
    if (!weather.data().valid) {
        _tft.setCursor(10, 40);
        _tft.print("Loading...");
    } else {
        const auto& w = weather.data();

        _tft.setCursor(10, 40);
        _tft.printf("Temp: %.1f C", w.temp);

        _tft.setCursor(10, 55);
        _tft.printf("Feels: %.1f C", w.feels);

        _tft.setCursor(10, 70);
        _tft.print(w.desc);
    }

    /* ===== BUTTONS ===== */
    _tft.drawFastHLine(0, _tft.height() - 14, _tft.width(), C_GRAY_40);
    _tft.setCursor(4, _tft.height() - 10);
    _tft.setTextColor(C_GRAY_60, bg);
    _tft.print("OK:update   BACK:clock");
}