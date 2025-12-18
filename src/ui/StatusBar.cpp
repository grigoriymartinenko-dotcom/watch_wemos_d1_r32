#include "ui/StatusBar.h"

void StatusBar::begin(Adafruit_ST7735& tft) {
    _tft = &tft;
}

uint16_t StatusBar::color(StatusColor s) {
    switch (s) {
        case StatusColor::OK:   return ST77XX_GREEN;
        case StatusColor::WARN: return ST77XX_YELLOW;
        default:                return ST77XX_RED;
    }
}

void StatusBar::setWiFi(StatusColor s) {
    if (_wifi != s) {
        _wifi = s;
        _dirty = true;
    }
}

void StatusBar::setApi(StatusColor s) {
    if (_api != s) {
        _api = s;
        _dirty = true;
    }
}

void StatusBar::setNtp(StatusColor s) {
    if (_ntp != s) {
        _ntp = s;
        _dirty = true;
    }
}

void StatusBar::setDate(uint8_t day, uint8_t month) {
    if (_day != day || _month != month) {
        _day = day;
        _month = month;
        _dirty = true;
    }
}
void StatusBar::draw() {
    if (!_tft || !_dirty) return;

    _dirty = false;

    _tft->fillRect(0, 0, _tft->width(), 12, ST77XX_BLACK);
    _tft->setTextSize(1);

    // WiFi
    _tft->setCursor(2, 2);
    _tft->setTextColor(color(_wifi));
    _tft->print("WiFi");

    // дата
    if (_day && _month) {
        char buf[6];
        snprintf(buf, sizeof(buf), "%02u.%02u", _day, _month);
        int16_t x = (_tft->width() - 6 * 6) / 2;
        _tft->setCursor(x, 2);
        _tft->setTextColor(ST77XX_WHITE);
        _tft->print(buf);
    }

    // NTP
    _tft->setCursor(_tft->width() - 28, 2);
    _tft->setTextColor(color(_ntp));
    _tft->print("NTP");
}