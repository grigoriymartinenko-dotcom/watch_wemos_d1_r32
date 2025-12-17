#include "screens/SettingsScreen.h"
#include "ui/Theme.h"

SettingsScreen::SettingsScreen(Adafruit_ST7735& tft, ScreenManager& sm)
    : _tft(tft), _sm(sm) {}

void SettingsScreen::begin() {
    _tft.fillScreen(C_UI_BG_NIGHT);

    switch (nightMode.state) {
        case NightModeState::AUTO: _modeIndex = 0; break;
        case NightModeState::ON:   _modeIndex = 1; break;
        case NightModeState::OFF:  _modeIndex = 2; break;
    }

    draw();
}

void SettingsScreen::update() {
    // ничего — реагируем только на кнопки
}

void SettingsScreen::onUp() {
    if (_modeIndex > 0) _modeIndex--;
    draw();
}

void SettingsScreen::onDown() {
    if (_modeIndex < 2) _modeIndex++;
    draw();
}

void SettingsScreen::onOk() {
    switch (_modeIndex) {
        case 0: nightMode.state = NightModeState::AUTO; break;
        case 1: nightMode.state = NightModeState::ON;   break;
        case 2: nightMode.state = NightModeState::OFF;  break;
    }
    draw();
}
    void SettingsScreen::onBack() {
    if (_clock) {
        _sm.set(_clock);
    }
}


void SettingsScreen::draw() {
    _tft.fillRect(0, 0, _tft.width(), _tft.height(), C_UI_BG_NIGHT);

    _tft.setTextColor(ST77XX_WHITE);
    _tft.setCursor(10, 20);
    _tft.print("Night Mode");

    _tft.setCursor(10, 50);
    _tft.print("> ");
    _tft.print(modeText());
}

const char* SettingsScreen::modeText() const {
    static const char* names[] = { "AUTO", "ON", "OFF" };
    return names[_modeIndex];
}