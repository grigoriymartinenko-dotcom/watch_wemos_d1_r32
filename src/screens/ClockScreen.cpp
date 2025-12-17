#include "screens/ClockScreen.h"
#include <Fonts/FreeSansBold18pt7b.h>
#include "ui/icons.h"
#include "ui/Theme.h"
#include "core/NightMode.h"
#include "sensors/DHTSensor.h"

extern DHTSensor dht;

/* ================= CONSTRUCTOR ================= */

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
    _lastNight = false;
}

/* ================= UPDATE ================= */

void ClockScreen::update() {
    RtcDateTime now = _rtc.GetDateTime();

    // ---- Night Mode AUTO logic ----
    bool autoNight = (now.Hour() >= 22 || now.Hour() < 7);

    switch (nightMode.state) {
        case NightModeState::AUTO:
            nightMode.isNight = autoNight;
            break;
        case NightModeState::ON:
            nightMode.isNight = true;
            break;
        case NightModeState::OFF:
            nightMode.isNight = false;
            break;
    }

    // ---- redraw on night switch ----
    if (_lastNight != nightMode.isNight) {
        _tft.fillScreen(
            nightMode.isNight ? C_UI_BG_NIGHT : C_UI_BG_DAY
        );
        _lastNight = nightMode.isNight;
        _lastDay = -1;
        _lastSec = -1;
    }

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

    drawClimate();
}

/* ================= DRAW TOP ================= */

void ClockScreen::drawTop(const RtcDateTime& dt) {
    uint16_t bg = nightMode.isNight ? C_UI_BG_NIGHT : C_UI_BG_DAY;
    uint16_t fg = nightMode.isNight ? C_TIME_DATE_NIGHT : C_TIME_DATE_DAY;

    _tft.fillRect(0, 0, _tft.width(), 18, bg);

    _tft.setTextColor(fg);
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

/* ================= TIME ================= */

void ClockScreen::drawTime(const RtcDateTime& dt) {
    uint16_t bg = nightMode.isNight ? C_UI_BG_NIGHT : C_UI_BG_DAY;
    uint16_t fg = nightMode.isNight ? C_TIME_MAIN_NIGHT : C_TIME_MAIN_DAY;

    _tft.fillRect(0, 22, _tft.width(), 46, bg);

    _tft.setFont(&FreeSansBold18pt7b);
    _tft.setTextColor(fg, bg);

    int x = 16;
    int y = 58;

    _tft.setCursor(x, y);
    _tft.printf("%02u", dt.Hour());

    _tft.setCursor(x + 44, y);
    _tft.print(_colonOn ? ":" : " ");

    _tft.setCursor(x + 58, y);
    _tft.printf("%02u", dt.Minute());

    _tft.setFont();
    _tft.drawFastHLine(0, 72, _tft.width(), C_GRAY_40);
}

/* ================= SECONDS ================= */

void ClockScreen::drawSeconds(const RtcDateTime& dt) {
    uint16_t bg = nightMode.isNight ? C_UI_BG_NIGHT : C_UI_BG_DAY;
    uint16_t fg = nightMode.isNight ? C_TIME_SECONDS_NIGHT : C_TIME_SECONDS_DAY;

    int x = 120;
    int y = 36;

    _tft.fillRect(x - 2, y - 2, 22, 14, bg);
    _tft.setTextColor(fg, bg);
    _tft.setCursor(x, y);
    _tft.printf("%02u", dt.Second());
}

/* ================= CLIMATE ================= */

void ClockScreen::drawClimate() {
    if (isnan(dht.temperature())) return;

    uint16_t fg = nightMode.isNight ? ST77XX_WHITE : ST77XX_BLACK;
    int y = 88;

    _tft.drawBitmap(10, y, ICON_TEMP, 8, 8, ST77XX_RED);
    _tft.setTextColor(fg);
    _tft.setCursor(22, y);
    _tft.printf("%dC", (int)dht.temperature());

    _tft.drawBitmap(80, y, ICON_HUM, 8, 8, ST77XX_CYAN);
    _tft.setCursor(92, y);
    _tft.printf("%d%%", (int)dht.humidity());
}

/* ================= HELPERS ================= */

const char* ClockScreen::dowShort(uint8_t dow) {
    static const char* names[] = {
        "Sun","Mon","Tue","Wed","Thu","Fri","Sat"
    };
    return names[dow % 7];
}
void ClockScreen::onUp() {
    // пока не используется
}

void ClockScreen::onDown() {
    // пока не используется
}

void ClockScreen::onOk() {
    if (_settings) {
        _sm.set(_settings);
    }
}

void ClockScreen::onBack() {
    // пока не используется
}