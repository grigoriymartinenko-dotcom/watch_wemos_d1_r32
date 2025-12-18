#include "screens/ClockScreen.h"
#include <Fonts/FreeSansBold24pt7b.h>
#include <Fonts/FreeSans9pt7b.h>

static const char* DOW[7] = {
    "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"
};

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
    _lastMinute = -1;
    _colonVisible = true;
    _lastBlinkMs = millis();

    _tft.fillScreen(ST77XX_BLACK);
    drawStatic();
}

void ClockScreen::update() {
    RtcDateTime now = _rtc.GetDateTime();

    int hh = now.Hour();
    int mm = now.Minute();

    // мигание двоеточия
    if (millis() - _lastBlinkMs >= 1000) {
        _lastBlinkMs = millis();
        _colonVisible = !_colonVisible;
        drawTime(hh, mm, _colonVisible);
    }

    // обновление минут → обновляем всё время и дату
    if (mm != _lastMinute) {
        _lastMinute = mm;
        drawTime(hh, mm, _colonVisible);
        drawDate(now);
    }
}

void ClockScreen::onOk() {
    if (_forecast) {
        _manager.set(_forecast);
    }
}

/* ===================== DRAW ===================== */

void ClockScreen::drawStatic() {
    // ничего статического кроме фона пока нет
}

void ClockScreen::drawTime(int hh, int mm, bool colon) {
    char buf[6];
    snprintf(buf, sizeof(buf),
             colon ? "%02d:%02d" : "%02d %02d",
             hh, mm);

    // очищаем только область времени
    _tft.fillRect(0, 20, _tft.width(), 50, ST77XX_BLACK);

    _tft.setFont(&FreeSansBold24pt7b);
    _tft.setTextColor(ST77XX_WHITE);
    _tft.setCursor(10, 60);
    _tft.print(buf);

    _tft.setFont(nullptr);
}

void ClockScreen::drawDate(const RtcDateTime& dt) {
    char buf[20];
    snprintf(buf, sizeof(buf),
             "%s %02u.%02u.%04u",
             DOW[dt.DayOfWeek()],
             dt.Day(),
             dt.Month(),
             dt.Year());

    // очищаем только строку даты
    _tft.fillRect(0, 80, _tft.width(), 20, ST77XX_BLACK);

    _tft.setFont(&FreeSans9pt7b);
    _tft.setTextColor(ST77XX_WHITE);
    _tft.setCursor(10, 98);
    _tft.print(buf);

    _tft.setFont(nullptr);
}