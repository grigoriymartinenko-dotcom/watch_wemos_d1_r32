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

    void setForecastScreen(Screen* s);

    // улица + дом
    void setOutdoorWeather(float tempC, int humidity);
    void setIndoorWeather(float tempC, int humidity);

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

    int digitHalfWidth();
    int _lastSecond = -1;
    // время
    int _lastMinute = -1;
    bool _colonVisible = true;
    uint32_t _lastBlinkMs = 0;

    // OUT
    float _outTempC = 1000.0f;
    int   _outHum   = -1;
    float _lastOutTempC = 1000.0f;
    int   _lastOutHum   = -1;

    // IN
    float _inTempC = 1000.0f;
    int   _inHum   = -1;
    float _lastInTempC = 1000.0f;
    int   _lastInHum   = -1;

    // draw helpers
    void drawSeconds(int ss);
    void drawHoursMinutes(int hh, int mm);
    void drawColon(bool visible);
    void drawDate(const RtcDateTime& dt);
    void drawWeather();
};