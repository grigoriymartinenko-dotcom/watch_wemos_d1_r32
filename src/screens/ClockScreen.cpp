#include "screens/ClockScreen.h"

ClockScreen::ClockScreen(
    Adafruit_ST7735& tft,
    RtcDS1302<ThreeWire>& rtc,
    ScreenManager& manager
)
: _tft(tft)
, _rtc(rtc)
, _manager(manager)
{}

void ClockScreen::setForecastScreen(Screen* s) {
    _forecast = s;
}

void ClockScreen::begin() {
    _redraw = true;
}

void ClockScreen::update() {
    if (!_redraw) return;

    _tft.fillScreen(ST77XX_BLACK);
    _tft.setTextSize(2);
    _tft.setTextColor(ST77XX_WHITE);
    _tft.setCursor(20, 40);
    _tft.print("CLOCK");

    _redraw = false;
}

void ClockScreen::onOk() {
    if (_forecast) {
        _manager.set(_forecast);
    }
}