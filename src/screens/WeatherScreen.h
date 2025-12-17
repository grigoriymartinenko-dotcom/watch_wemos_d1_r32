#pragma once
#include <Adafruit_ST7735.h>

#include "core/Screen.h"
#include "core/ScreenManager.h"

/*
 * WeatherScreen (baseline)
 * =======================
 * Пока это заглушка UI, чтобы проект собирался.
 * Позже подключим WeatherService и OpenWeather.
 *
 * Навигация:
 * - BACK -> Clock
 */
class WeatherScreen : public Screen {
public:
    WeatherScreen(Adafruit_ST7735& tft, ScreenManager& sm) : _tft(tft), _sm(sm) {}

    void setBack(Screen* clock) { _clock = clock; }

    void begin() override;
    void update() override {}

    void onBack() override;

private:
    Adafruit_ST7735& _tft;
    ScreenManager& _sm;
    Screen* _clock = nullptr;
};
