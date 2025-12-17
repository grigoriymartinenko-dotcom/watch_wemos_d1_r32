#pragma once

#include "core/Screen.h"
#include "core/ScreenManager.h"
#include "ui/Theme.h"

#include <RtcDS1302.h>

/*
 * ClockScreen
 * ===========
 * Главный экран часов
 *
 * Зоны:
 *  ┌──────── TOP ────────┐  WiFi | Date | NTP
 *  ├─────────────────────┤
 *  │        HH:MM        │  КРУПНЫЙ шрифт
 *  │        sec          │
 *  ├─────────────────────┤  ← линия
 *  │   temp / humidity   │  (ПОКА ПУСТО)
 *  └─────────────────────┘
 */

class ClockScreen : public Screen {
public:
    ClockScreen(Adafruit_ST7735& tft,
                RtcDS1302<ThreeWire>& rtc,
                ScreenManager& sm);

    void setLinks(Screen* weather, Screen* settings);

    void begin() override;
    void update() override;

    /* КНОПКИ НА ЭТОМ ЭКРАНЕ НЕ НУЖНЫ */
    void drawButtons(Adafruit_ST7735&) override {}

private:
    Adafruit_ST7735&      _tft;
    RtcDS1302<ThreeWire>& _rtc;
    ScreenManager&        _sm;

    Screen* _weather  = nullptr;
    Screen* _settings = nullptr;
    
    int _lastTemp = -100;
    int _lastHum  = -100;

    int  _lastSec = -1;
    int  _lastDay = -1;
    bool _colonOn = true;

    void drawTop(const RtcDateTime& dt);
    void drawTime(const RtcDateTime& dt);
    void drawSeconds(const RtcDateTime& dt);

    const char* dowShort(uint8_t dow);
    void drawClimate();
};