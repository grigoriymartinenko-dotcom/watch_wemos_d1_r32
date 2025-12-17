#pragma once
#include <Adafruit_ST7735.h>
#include "core/Screen.h"

/*
 * ScreenManager (NEW API)
 * =======================
 * - set(): переключение экрана
 * - update(): вызывает update() активного экрана + подписи кнопок
 * - onUp/onDown/onOk/onBack: проксирование кнопок на активный экран
 */
class ScreenManager {
public:
    void set(Screen* s) {
        _current = s;
        if (_current) _current->begin();
    }

    Screen* active() const { return _current; }

void update() {
    if (_current) {
        _current->update();
    }
}

    void onUp()   { if (_current) _current->onUp(); }
    void onDown() { if (_current) _current->onDown(); }
    void onOk()   { if (_current) _current->onOk(); }
    void onBack() { if (_current) _current->onBack(); }

private:
    Screen* _current = nullptr;
};
