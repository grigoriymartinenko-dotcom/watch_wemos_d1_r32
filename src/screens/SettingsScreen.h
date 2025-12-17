#pragma once
#include <Adafruit_ST7735.h>
#include "core/Screen.h"
#include "core/ScreenManager.h"
#include "core/NightMode.h"

class SettingsScreen : public Screen {
public:
    SettingsScreen(Adafruit_ST7735& tft, ScreenManager& sm);

    void begin() override;
    void update() override;

    void onUp() override;
    void onDown() override;
    void onOk() override;
    void onBack() override;
Screen* _clock = nullptr;
public:
    void setClock(Screen* clock) { _clock = clock; }
private:
    Adafruit_ST7735& _tft;
    ScreenManager& _sm;

    int _modeIndex = 0; // 0=AUTO, 1=ON, 2=OFF

    void draw();
    const char* modeText() const;
};