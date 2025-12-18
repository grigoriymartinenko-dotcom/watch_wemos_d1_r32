#pragma once
#include <Adafruit_ST7735.h>
#include <RtcDS1302.h>
#include "core/Screen.h"
#include "core/ScreenManager.h"

class ClockScreen : public Screen {
public:
    ClockScreen(Adafruit_ST7735& tft,
                RtcDS1302<ThreeWire>& rtc,
                ScreenManager& manager);

    // связываем ForecastScreen ПОСЛЕ создания объектов
    void setForecastScreen(Screen* s);

    void begin() override;
    void update() override;
    void onOk() override;

    // кнопки не рисуем
    void drawButtons(Adafruit_ST7735&) override {}

private:
    Adafruit_ST7735& _tft;
    RtcDS1302<ThreeWire>& _rtc;
    ScreenManager& _manager;
    Screen* _forecast = nullptr;

    bool _redraw = true;
};