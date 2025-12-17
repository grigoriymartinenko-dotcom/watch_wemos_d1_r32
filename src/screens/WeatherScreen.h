#pragma once
#include <Adafruit_ST7735.h>
#include "core/Screen.h"
#include "core/ScreenManager.h"

class WeatherScreen : public Screen {
public:
    WeatherScreen(Adafruit_ST7735& tft, ScreenManager& sm);

    void setClock(Screen* clock);

    void begin() override;
    void update() override;

    void onUp() override;
    void onDown() override;
    void onOk() override;
    void onBack() override;

private:
    enum class View {
        NOW,
        FORECAST
    };

    Adafruit_ST7735& _tft;
    ScreenManager& _sm;
    Screen* _clock = nullptr;

    View _view = View::NOW;
    int  _dayIndex = 0;

    void draw();
    void drawNow();
    void drawForecast();
};