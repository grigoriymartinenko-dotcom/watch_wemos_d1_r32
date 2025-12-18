#pragma once

#include <Arduino.h>
#include <Adafruit_ST7735.h>   // ⬅️ ВОТ ЭТОГО НЕ ХВАТАЛО
#include <stdint.h>

enum class WeatherIcon : uint8_t {
    CLEAR,
    CLOUDS,
    RAIN,
    STORM,
    SNOW,
    FOG,
    UNKNOWN
};

inline WeatherIcon iconFromWeatherId(uint16_t id) {
    if (id >= 200 && id < 300) return WeatherIcon::STORM;
    if (id >= 300 && id < 600) return WeatherIcon::RAIN;
    if (id >= 600 && id < 700) return WeatherIcon::SNOW;
    if (id >= 700 && id < 800) return WeatherIcon::FOG;
    if (id == 800)             return WeatherIcon::CLEAR;
    if (id > 800)              return WeatherIcon::CLOUDS;
    return WeatherIcon::UNKNOWN;
}

// ОБЪЯВЛЕНИЕ ФУНКЦИИ
void drawWeatherIcon(Adafruit_ST7735& tft,
                     int x,
                     int y,
                     WeatherIcon icon);