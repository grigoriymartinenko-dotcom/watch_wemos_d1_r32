#include "services/ForecastService.h"
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <time.h>
#include <string.h>

static const char* OPENWEATHER_KEY = "07108cf067a5fdf5aa26dce75354400f";
static const char* CITY = "Kharkiv";
static const char* UNITS = "metric";
static const char* LANG  = "en";

void ForecastService::begin() {
    _lastFetch = 0;
    resetData();
}

const ForecastData& ForecastService::data() const {
    return _data;
}

void ForecastService::resetData() {
    _data.valid = false;
    _data.count = 0;
    _data.ts = 0;

    for (int i = 0; i < 5; i++) {
        _data.days[i].date[0] = '\0';
        _data.days[i].minTemp = 127;
        _data.days[i].maxTemp = -127;
        _data.days[i].nightTemp = 127;
        _data.days[i].dayWeatherId = 0;
        _data.days[i].nightWeatherId = 0;
    }
}

bool ForecastService::update(bool force) {
    Serial.println("[Forecast] update() called");

    if (!WiFi.isConnected()) {
        Serial.println("[Forecast] WiFi NOT connected");
        return false;
    }

    unsigned long nowMs = millis();
    if (!force && _data.valid && (nowMs - _lastFetch < CACHE_MS)) {
        Serial.println("[Forecast] cache valid");
        return false;
    }

    Serial.println("[Forecast] fetching from API...");
    bool ok = fetchFromApi();

    Serial.print("[Forecast] fetch result = ");
    Serial.println(ok ? "OK" : "FAIL");

    if (ok) _lastFetch = nowMs;
    return ok;
}

/* =========================================================
 * CORE FETCH — НЕ ЗАВИСИТ ОТ ВРЕМЕНИ ESP32
 * ========================================================= */
bool ForecastService::fetchFromApi() {
    HTTPClient http;

    String url =
        String("http://api.openweathermap.org/data/2.5/forecast?q=") +
        CITY + "&appid=" + OPENWEATHER_KEY +
        "&units=" + UNITS +
        "&lang=" + LANG;

    http.begin(url);
    int code = http.GET();

    Serial.print("[Forecast] HTTP code: ");
    Serial.println(code);

    if (code != HTTP_CODE_OK) {
        http.end();
        return false;
    }

    DynamicJsonDocument doc(48 * 1024);
    DeserializationError err = deserializeJson(doc, http.getStream());
    http.end();

    if (err) {
        Serial.print("[Forecast] JSON error: ");
        Serial.println(err.c_str());
        return false;
    }

    resetData();

    JsonArray list = doc["list"].as<JsonArray>();
    if (list.isNull()) return false;

    // timezone города (секунды)
    int cityTz = doc["city"]["timezone"] | 0;

    int bestDayDiff[5];   for (int i = 0; i < 5; i++) bestDayDiff[i] = 99;
    int bestNightDiff[5]; for (int i = 0; i < 5; i++) bestNightDiff[i] = 99;

    int baseYday = -1;

    for (JsonVariant v : list) {
        time_t t = (time_t)(v["dt"].as<long>() + cityTz);
        struct tm tm;
        gmtime_r(&t, &tm);

        if (baseYday < 0) baseYday = tm.tm_yday;

        int dayIdx = tm.tm_yday - baseYday;
        if (dayIdx < 0 || dayIdx >= 5) continue;

        // первая инициализация дня
        if (_data.days[dayIdx].date[0] == '\0') {
            strftime(_data.days[dayIdx].date,
                     sizeof(_data.days[dayIdx].date),
                     "%Y-%m-%d", &tm);
            if (_data.count < dayIdx + 1)
                _data.count = dayIdx + 1;
        }

        int8_t tMin = (int8_t)roundf(v["main"]["temp_min"].as<float>());
        int8_t tMax = (int8_t)roundf(v["main"]["temp_max"].as<float>());
        int8_t tCur = (int8_t)roundf(v["main"]["temp"].as<float>());

        uint16_t wid = v["weather"][0]["id"] | 804;

        if (tMin < _data.days[dayIdx].minTemp) _data.days[dayIdx].minTemp = tMin;
        if (tMax > _data.days[dayIdx].maxTemp) _data.days[dayIdx].maxTemp = tMax;

        // DAY ≈ 15:00
        int dayDiff = abs(tm.tm_hour - 15);
        if (dayDiff < bestDayDiff[dayIdx]) {
            bestDayDiff[dayIdx] = dayDiff;
            _data.days[dayIdx].dayWeatherId = wid;
        }

        // NIGHT ≈ 03:00
        int nightDiff = abs(tm.tm_hour - 3);
        if (nightDiff < bestNightDiff[dayIdx]) {
            bestNightDiff[dayIdx] = nightDiff;
            _data.days[dayIdx].nightWeatherId = wid;
            _data.days[dayIdx].nightTemp = tCur;
        }
    }

    // финализация
    for (int i = 0; i < _data.count; i++) {
        if (_data.days[i].nightTemp == 127) {
            _data.days[i].nightTemp =
                (_data.days[i].minTemp + _data.days[i].maxTemp) / 2;
        }
        if (_data.days[i].dayWeatherId == 0)
            _data.days[i].dayWeatherId = 804;
        if (_data.days[i].nightWeatherId == 0)
            _data.days[i].nightWeatherId = 804;
    }

    _data.valid = (_data.count > 0);
    _data.ts = millis();
    return _data.valid;
}