#include "screens/ClockScreen.h"
#include <Fonts/FreeSansBold18pt7b.h>
#include "ui/icons.h"
#include "sensors/DHTSensor.h"
#include "ClockScreen.h"

extern DHTSensor dht;

/* ================= КОНСТРУКТОР ================= */

ClockScreen::ClockScreen(Adafruit_ST7735& tft,
                         RtcDS1302<ThreeWire>& rtc,
                         ScreenManager& sm)
: _tft(tft), _rtc(rtc), _sm(sm) {}

void ClockScreen::setLinks(Screen* weather, Screen* settings) {
    _weather  = weather;
    _settings = settings;
}

/* ================= BEGIN ================= */

void ClockScreen::begin() {
    _tft.fillScreen(C_UI_BG_NIGHT);
    _lastSec = -1;
    _lastDay = -1;
    
}

/* ================= UPDATE ================= */

void ClockScreen::update() {
    RtcDateTime now = _rtc.GetDateTime();

    if (now.Day() != _lastDay) {
        drawTop(now);
        _lastDay = now.Day();
    }

    if (now.Second() != _lastSec) {
        _colonOn = !_colonOn;
        drawTime(now);
        drawSeconds(now);
        _lastSec = now.Second();
    }

    drawClimate(); // теперь умный и лёгкий
}
/* ================= TOP ================= */

void ClockScreen::drawTop(const RtcDateTime& dt) {
    _tft.fillRect(0, 0, _tft.width(), 18, C_UI_BG_NIGHT);

    _tft.setTextColor(C_TIME_DATE_NIGHT);
    _tft.setCursor(2, 4);
    _tft.print("WiFi");

    _tft.setCursor(40, 4);
    _tft.printf("%s %02u.%02u.%04u",
        dowShort(dt.DayOfWeek()),
        dt.Day(),
        dt.Month(),
        dt.Year()
    );

    _tft.setCursor(_tft.width() - 28, 4);
    _tft.print("NTP");

    _tft.drawFastHLine(0, 18, _tft.width(), C_GRAY_40);
}

/* ================= TIME (КРУПНО) ================= */

void ClockScreen::drawTime(const RtcDateTime& dt) {
    uint16_t bg = C_UI_BG_NIGHT;
    uint16_t fg = C_TIME_MAIN_NIGHT;

    // очищаем зону времени
    _tft.fillRect(0, 22, _tft.width(), 46, bg);

    // крупный шрифт ТОЛЬКО тут
    _tft.setFont(&FreeSansBold18pt7b);
    _tft.setTextColor(fg, bg);

    int x = 16;
    int y = 58;   // baseline под 18pt

    _tft.setCursor(x, y);
    _tft.printf("%02u", dt.Hour());

    _tft.setCursor(x + 44, y);
    _tft.print(_colonOn ? ":" : " ");

    _tft.setCursor(x + 58, y);
    _tft.printf("%02u", dt.Minute());

    // ВОЗВРАТ к стандартному шрифту
    _tft.setFont();

    // линия под временем (с отступом)
    _tft.drawFastHLine(0, 72, _tft.width(), C_GRAY_40);
}

/* ================= SECONDS ================= */

void ClockScreen::drawSeconds(const RtcDateTime& dt) {
    uint16_t fg = C_TIME_SECONDS_NIGHT;
    uint16_t bg = C_UI_BG_NIGHT;

    int x = 120;
    int y = 36;   // выше, как "значок"

    _tft.fillRect(x - 2, y - 2, 22, 14, bg);

    _tft.setTextColor(fg, bg);
    _tft.setCursor(x, y);
    _tft.printf("%02u", dt.Second());
}

/* ================= HELPERS ================= */

const char* ClockScreen::dowShort(uint8_t dow) {
    static const char* names[] = {
        "Sun","Mon","Tue","Wed","Thu","Fri","Sat"
    };
    return names[dow % 7];
}
void ClockScreen::drawClimate() {
    if (isnan(dht.temperature())) return;

    int t = (int)dht.temperature();
    int h = (int)dht.humidity();

    // если ничего не изменилось — не рисуем
    if (t == _lastTemp && h == _lastHum) return;

    _lastTemp = t;
    _lastHum  = h;

    const int y = 88;
    const int hZone = 12;

    // очищаем зону климата
    _tft.fillRect(0, y - 1, _tft.width(), hZone, C_UI_BG_NIGHT);

    // --- 🌡 Температура ---
    _tft.drawBitmap(10, y, ICON_TEMP, 8, 8, ST77XX_RED);
    _tft.setTextColor(ST77XX_WHITE, C_UI_BG_NIGHT);
    _tft.setTextSize(1);
    _tft.setCursor(22, y);
    _tft.printf("%dC", t);

    // --- 💧 Влажность ---
    _tft.drawBitmap(80, y, ICON_HUM, 8, 8, ST77XX_CYAN);
    _tft.setCursor(92, y);
    _tft.printf("%d%%", h);
}