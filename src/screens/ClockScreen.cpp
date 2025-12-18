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

void ClockScreen::setOutdoorWeather(float tempC, int humidity) {
    _outTempC = tempC;
    _outHum   = humidity;
}

void ClockScreen::setIndoorWeather(float tempC, int humidity) {
    _inTempC = tempC;
    _inHum   = humidity;
}

void ClockScreen::begin() {
    _lastMinute = -1;
    _colonVisible = true;
    _lastBlinkMs = millis();

    _tft.fillScreen(ST77XX_BLACK);
}

void ClockScreen::update() {
    RtcDateTime now = _rtc.GetDateTime();

    int ss = now.Second();

// секунды — отдельно
if (ss != _lastSecond) {
    _lastSecond = ss;
    drawSeconds(ss);
}

    int hh = now.Hour();
    int mm = now.Minute();

    // 1️⃣ смена минуты → перерисовываем HH и MM
    if (mm != _lastMinute) {
        _lastMinute = mm;
        drawHoursMinutes(hh, mm);
        drawDate(now);
    }

    // 2️⃣ мигание ТОЛЬКО двоеточия
    if (millis() - _lastBlinkMs >= 1000) {
        _lastBlinkMs = millis();
        _colonVisible = !_colonVisible;
        drawColon(_colonVisible);
    }

    // 3️⃣ погода (OUT + IN)
    if (_outTempC != _lastOutTempC || _outHum != _lastOutHum ||
        _inTempC  != _lastInTempC  || _inHum  != _lastInHum) {

        drawWeather();

        _lastOutTempC = _outTempC;
        _lastOutHum   = _outHum;
        _lastInTempC  = _inTempC;
        _lastInHum    = _inHum;
    }
}

void ClockScreen::onOk() {
    if (_forecast) {
        _manager.set(_forecast);
    }
}

/* ===================== DRAW ===================== */

void ClockScreen::drawHoursMinutes(int hh, int mm) {
    char buf[3];

    // очищаем строку времени
    _tft.fillRect(0, 20, _tft.width(), 50, ST77XX_BLACK);

    _tft.setFont(&FreeSansBold24pt7b);
    _tft.setTextColor(ST77XX_WHITE);

    // HH
    snprintf(buf, sizeof(buf), "%02d", hh);
    _tft.setCursor(12, 60);
    _tft.print(buf);

    // MM
    snprintf(buf, sizeof(buf), "%02d", mm);
    _tft.setCursor(96, 60);
    _tft.print(buf);

    _tft.setFont(nullptr);
}

void ClockScreen::drawColon(bool visible) {
    _tft.setFont(&FreeSansBold24pt7b);

    const int colonX = 72;
    const int colonY = 60;

    if (visible) {
        _tft.setTextColor(ST77XX_WHITE);
        _tft.setCursor(colonX, colonY);
        _tft.print(":");
    } else {
        // стираем только двоеточие
        _tft.fillRect(colonX - 4, 30, 16, 40, ST77XX_BLACK);
    }

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

    _tft.fillRect(0, 80, _tft.width(), 20, ST77XX_BLACK);

    _tft.setFont(&FreeSans9pt7b);
    _tft.setTextColor(ST77XX_WHITE);
    _tft.setCursor(10, 98);
    _tft.print(buf);

    _tft.setFont(nullptr);
}

void ClockScreen::drawWeather() {
    _tft.fillRect(0, 102, _tft.width(), 34, ST77XX_BLACK);

    _tft.setFont(&FreeSans9pt7b);

    // OUT
    _tft.setTextColor(ST77XX_CYAN);
    _tft.setCursor(6, 118);
    _tft.print("OUT:");

    _tft.setTextColor(ST77XX_WHITE);
    if (_outHum >= 0 && _outTempC < 900) {
        _tft.print(" ");
        _tft.print(_outTempC, 1);
        _tft.print((char)247);
        _tft.print("C ");
        _tft.print(_outHum);
        _tft.print("%");
    } else {
        _tft.print(" --");
    }

    // IN
    _tft.setTextColor(ST77XX_GREEN);
    _tft.setCursor(6, 134);
    _tft.print("IN:");

    _tft.setTextColor(ST77XX_WHITE);
    if (_inHum >= 0 && _inTempC < 900) {
        _tft.print(" ");
        _tft.print(_inTempC, 1);
        _tft.print((char)247);
        _tft.print("C ");
        _tft.print(_inHum);
        _tft.print("%");
    } else {
        _tft.print(" --");
    }

    _tft.setFont(nullptr);
}
void ClockScreen::drawSeconds(int ss) {
    char buf[3];
    snprintf(buf, sizeof(buf), "%02d", ss);

    // зона секунд (справа от MM)
    _tft.fillRect(130, 42, 30, 20, ST77XX_BLACK);

    _tft.setFont(&FreeSans9pt7b);
    _tft.setTextColor(ST77XX_WHITE);
    _tft.setCursor(132, 58);
    _tft.print(buf);

    _tft.setFont(nullptr);
}