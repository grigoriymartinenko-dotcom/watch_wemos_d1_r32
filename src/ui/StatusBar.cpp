#include "ui/StatusBar.h"
#include <string.h>

static const char* DOW[7] = {
    "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"
};

void StatusBar::begin(Adafruit_ST7735& tft) {
    _tft = &tft;
    _dirty = true;
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

void StatusBar::setNtp(StatusColor s) {
    if (_ntp != s) {
        _ntp = s;
        _dirty = true;
    }
}

void StatusBar::setDate(uint8_t dow, uint8_t day, uint8_t month, uint16_t year) {
    if (_dow != dow || _day != day || _month != month || _year != year) {
        _dow   = dow;
        _day   = day;
        _month = month;
        _year  = year;
        _dirty = true;
    }
}

void StatusBar::draw() {
    if (!_tft || !_dirty) return;
    _dirty = false;

    _tft->fillRect(0, 0, _tft->width(), 12, ST77XX_BLACK);
    _tft->setTextSize(1);

    // ---- WiFi (слева) ----
    _tft->setCursor(2, 2);
    _tft->setTextColor(color(_wifi));
    _tft->print("WiFi");

    // ---- центр: Thu 18.12.2025 ----
    if (_day && _month && _year) {
        char buf[20];
        snprintf(buf, sizeof(buf),
                 "%s %02u.%02u.%04u",
                 DOW[_dow],
                 _day,
                 _month,
                 _year);

        int16_t x = (_tft->width() - strlen(buf) * 6) / 2;
        _tft->setCursor(x, 2);
        _tft->setTextColor(ST77XX_WHITE);
        _tft->print(buf);
    }

    // ---- NTP (справа) ----
    _tft->setCursor(_tft->width() - 28, 2);
    _tft->setTextColor(color(_ntp));
    _tft->print("NTP");
}