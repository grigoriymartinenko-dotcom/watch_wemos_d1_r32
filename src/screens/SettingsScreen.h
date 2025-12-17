#pragma once
#include <Adafruit_ST7735.h>

#include "core/Screen.h"
#include "core/ScreenManager.h"
#include "settings/ClockNightMode.h"

/*
 * SettingsScreen (baseline)
 * ========================
 * Минимальные настройки:
 *  - Night mode cycle: DAY -> NIGHT -> AUTO -> DAY
 *
 * Навигация:
 * - UP/DOWN: выбор пункта (пока один пункт)
 * - OK: переключить
 * - BACK: назад в Clock
 */
class SettingsScreen : public Screen {
public:
    SettingsScreen(Adafruit_ST7735& tft, ScreenManager& sm) : _tft(tft), _sm(sm) {}

    void setBack(Screen* clock) { _clock = clock; }

    void begin() override;
    void update() override {}

    void onOk() override;
    void onBack() override;

private:
    void draw();

    Adafruit_ST7735& _tft;
    ScreenManager& _sm;
    Screen* _clock = nullptr;
};
