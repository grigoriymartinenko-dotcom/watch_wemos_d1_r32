#include "ui/StatusBar.h"

void StatusBar::begin(Adafruit_ST7735& tft) {
    _tft = &tft;
}

uint16_t StatusBar::color(StatusColor s) {
    switch (s) {
        case StatusColor::OK:    return ST77XX_GREEN;
        case StatusColor::WARN:  return ST77XX_YELLOW;
        default:                 return ST77XX_RED;
    }
}

void StatusBar::setWiFi(StatusColor s) { _wifi = s; }
void StatusBar::setApi(StatusColor s)  { _api  = s; }
void StatusBar::setRtc(StatusColor s)  { _rtc  = s; }

void StatusBar::draw() {
    if (!_tft) return;

    _tft->fillRect(0, 0, _tft->width(), 10, ST77XX_BLACK);
    _tft->setTextSize(1);

    _tft->setCursor(4, 2);
    _tft->setTextColor(color(_wifi));
    _tft->print("W");

    _tft->setCursor(20, 2);
    _tft->setTextColor(color(_api));
    _tft->print("A");

    _tft->setCursor(36, 2);
    _tft->setTextColor(color(_rtc));
    _tft->print("R");
}