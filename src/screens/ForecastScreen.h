#pragma once
#include <Adafruit_ST7735.h>
#include "core/Screen.h"
#include "core/ScreenManager.h"
#include "services/ForecastService.h"

class ForecastScreen : public Screen {
public:
    ForecastScreen(Adafruit_ST7735& tft,
                   ForecastService& forecast,
                   ScreenManager& manager,
                   Screen* backScreen);

    void begin() override;
    void update() override;
    void onBack() override;

    void drawButtons(Adafruit_ST7735&) override {}

private:
    Adafruit_ST7735& _tft;
    ForecastService& _forecast;
    ScreenManager& _manager;
    Screen* _back;

    bool _redraw = true;

    void drawHeader();
    void drawContent();
};