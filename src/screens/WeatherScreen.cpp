#include "screens/WeatherScreen.h"
#include "ui/Theme.h"
#include "ui/weather_icons.h"
#include "services/WeatherService.h"
#include "services/ForecastService.h"
#include <WiFi.h>
#include <time.h>

extern WeatherService weather;
extern ForecastService forecast;

/* =========================================================
 * ICON MAP
 * ========================================================= */
static const uint8_t* iconForWeather(uint16_t id) {
    if (id >= 200 && id < 600) return ICON_RAIN;
    if (id >= 600 && id < 700) return ICON_SNOW;
    if (id >= 700 && id < 800) return ICON_MIST;
    if (id == 800)             return ICON_SUN;
    if (id > 800)              return ICON_CLOUD;
    return ICON_CLOUD;
}

/* =========================================================
 * BOTTOM BUTTONS — 2 ROWS
 * ========================================================= */
static void drawButtons2Row(Adafruit_ST7735& tft, uint16_t bg,
                            const char* l1, const char* l2) {
    int y = tft.height() - 26;
    tft.fillRect(0, y, tft.width(), 26, bg);
    tft.drawFastHLine(0, y, tft.width(), C_GRAY_40);

    tft.setTextColor(C_GRAY_60, bg);
    tft.setCursor(4, y + 6);
    tft.print(l1);
    tft.setCursor(4, y + 16);
    tft.print(l2);
}

/* =========================================================
 * DATE FORMAT: "Mon 18 Mar"
 * ========================================================= */
static void formatDate(const char* ymd, char* out, size_t len) {
    // ymd = "YYYY-MM-DD"
    struct tm tm{};
    tm.tm_year = (ymd[0]-'0')*1000 + (ymd[1]-'0')*100 +
                 (ymd[2]-'0')*10   + (ymd[3]-'0') - 1900;
    tm.tm_mon  = (ymd[5]-'0')*10 + (ymd[6]-'0') - 1;
    tm.tm_mday = (ymd[8]-'0')*10 + (ymd[9]-'0');

    strftime(out, len, "%a %d %b", &tm);
}

/* =========================================================
 * CTOR / LINKS
 * ========================================================= */
WeatherScreen::WeatherScreen(Adafruit_ST7735& tft, ScreenManager& sm)
    : _tft(tft), _sm(sm) {}

void WeatherScreen::setClock(Screen* clock) {
    _clock = clock;
}

/* =========================================================
 * LIFECYCLE
 * ========================================================= */
void WeatherScreen::begin() {
    _view = View::NOW;
    _dayIndex = 0;
    _tft.fillScreen(C_UI_BG_NIGHT);
    draw();
}

/*
 * ВАЖНО:
 * update() — НИЧЕГО не грузит, НИЧЕГО не чистит.
 * Только UI. Это убивает белые экраны на 100%.
 */
void WeatherScreen::update() {
    // intentionally empty
}

/* =========================================================
 * BUTTONS
 * ========================================================= */
void WeatherScreen::onOk() {
    if (_view == View::NOW) {
        weather.update();          // принудительно
    } else {
        forecast.update(true);     // принудительно
    }
    draw();
}

void WeatherScreen::onBack() {
    if (_clock) {
        _sm.set(_clock);           // НИКАКИХ лишних begin()
    }
}

void WeatherScreen::onDown() {
    if (_view == View::NOW) {
        _view = View::FORECAST;
        _dayIndex = 0;
        forecast.update(true);     // ГАРАНТИРОВАННО грузим прогноз
        draw();
        return;
    }

    const auto& f = forecast.data();
    if (_dayIndex < (int)f.count - 1) {
        _dayIndex++;
        draw();
    }
}

void WeatherScreen::onUp() {
    if (_view == View::FORECAST) {
        if (_dayIndex == 0) {
            _view = View::NOW;
            draw();                // БЕЗ update()
            return;
        }
        _dayIndex--;
        draw();
    }
}

/* =========================================================
 * DRAW ROUTER
 * ========================================================= */
void WeatherScreen::draw() {
    if (_view == View::NOW) drawNow();
    else drawForecast();
}

/* =========================================================
 * NOW
 * ========================================================= */
void WeatherScreen::drawNow() {
    uint16_t bg = C_UI_BG_NIGHT;
    _tft.fillScreen(bg);
    _tft.setTextColor(ST77XX_WHITE, bg);

    _tft.setCursor(4, 4);
    _tft.print("Weather NOW");

    _tft.setCursor(_tft.width() - 40, 4);
    _tft.setTextColor(WiFi.isConnected() ? ST77XX_GREEN : ST77XX_RED, bg);
    _tft.print(WiFi.isConnected() ? "WiFi" : "NoWi");
    _tft.setTextColor(ST77XX_WHITE, bg);

    _tft.drawFastHLine(0, 18, _tft.width(), C_GRAY_40);

    if (!weather.data().valid) {
        _tft.setCursor(10, 40);
        _tft.print("Loading...");
        drawButtons2Row(_tft, bg,
            "DOWN: forecast",
            "OK: update   BACK: clock");
        return;
    }

    const auto& w = weather.data();

    _tft.drawBitmap(10, 30, iconForWeather(w.weatherId), 16, 16, ST77XX_YELLOW);
    _tft.setCursor(32, 32);
    _tft.printf("%.1f C", w.temp);
    _tft.setCursor(32, 48);
    _tft.printf("Feels %.1f", w.feels);

    _tft.setCursor(10, 70);
    _tft.print(w.desc);

    drawButtons2Row(_tft, bg,
        "DOWN: forecast",
        "OK: update   BACK: clock");
}

/* =========================================================
 * FORECAST
 * ========================================================= */
void WeatherScreen::drawForecast() {
    uint16_t bg = C_UI_BG_NIGHT;
    _tft.fillScreen(bg);
    _tft.setTextColor(ST77XX_WHITE, bg);

    _tft.setCursor(4, 4);
    _tft.print("Forecast");

    _tft.drawFastHLine(0, 18, _tft.width(), C_GRAY_40);

    const auto& f = forecast.data();
    if (!f.valid || f.count == 0) {
        _tft.setCursor(10, 40);
        _tft.print("Fetching forecast...");
        drawButtons2Row(_tft, bg,
            "UP/DOWN: days",
            "OK: update   BACK: clock");
        return;
    }

    int idx = _dayIndex;
    if (idx >= (int)f.count) idx = f.count - 1;

    const auto& d = f.days[idx];

    char dateBuf[20];
    formatDate(d.date, dateBuf, sizeof(dateBuf));

    _tft.setCursor(10, 26);
    _tft.print(dateBuf);

    // DAY
    _tft.drawBitmap(10, 44, iconForWeather(d.dayWeatherId), 16, 16, ST77XX_YELLOW);
    _tft.setCursor(32, 46);
    _tft.printf("Day   %dC", d.maxTemp);

    // NIGHT
    _tft.drawBitmap(10, 66, iconForWeather(d.nightWeatherId), 16, 16, ST77XX_CYAN);
    _tft.setCursor(32, 68);
    _tft.printf("Night %dC", d.nightTemp);

    // index
    _tft.setCursor(_tft.width() - 26, 26);
    _tft.printf("%d/%d", idx + 1, f.count);

    drawButtons2Row(_tft, bg,
        "UP/DOWN: days",
        "OK: update   BACK: clock");
}