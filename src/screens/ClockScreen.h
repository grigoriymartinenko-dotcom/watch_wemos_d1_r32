#pragma once
#include <Adafruit_ST7735.h>
#include <RtcDS1302.h>
#include "core/Screen.h"
#include "core/ScreenManager.h"

class ClockScreen : public Screen {
public:
    ClockScreen(Adafruit_ST7735& tft,
                RtcDS1302<ThreeWire>& rtc,
                ScreenManager& sm);

    void setLinks(Screen* weather, Screen* settings);

    void begin() override;
    void update() override;

    void onUp() override;
    void onDown() override;
    void onOk() override;
    void onBack() override;

private:
    Adafruit_ST7735& _tft;
    RtcDS1302<ThreeWire>& _rtc;
    ScreenManager& _sm;

    Screen* _weather  = nullptr;
    Screen* _settings = nullptr;

    int  _lastSec = -1;
    int  _lastDay = -1;
    bool _colonOn = true;
    bool _lastNight = false;

    // --- climate cache ---
    int _lastTemp = -1000;
    int _lastHum  = -1000;

    void drawTop(const RtcDateTime& dt);
    void drawTime(const RtcDateTime& dt);
    void drawSeconds(const RtcDateTime& dt);
    void drawClimate(bool force = false);

    const char* dowShort(uint8_t dow);
};