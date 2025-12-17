#pragma once
#include <Adafruit_ST7735.h>
#include "core/Screen.h"
#include "core/ScreenManager.h"

class SettingsScreen : public Screen {
public:
    SettingsScreen(Adafruit_ST7735& tft, ScreenManager& sm);

    void setClock(Screen* clock);

    void begin() override;
    void update() override;

    void onUp() override;
    void onDown() override;
    void onOk() override;
    void onBack() override;

private:
    Adafruit_ST7735& _tft;
    ScreenManager& _sm;
    Screen* _clock = nullptr;

    uint8_t _modeIndex = 0;

    void draw();
    const char* modeText() const;
};