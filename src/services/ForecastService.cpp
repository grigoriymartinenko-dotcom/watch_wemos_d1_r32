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

    JsonArray list = doc["list"].as<JsonArray>();
    if (list.isNull() || list.size() == 0) {
        Serial.println("[Forecast] list empty");
        return false;
    }

    // ─── ВРЕМЕННЫЕ ДАННЫЕ ─────────────────────────────
    ForecastData tmp;
    memset(&tmp, 0, sizeof(tmp));
    tmp.count = 0;

    for (int i = 0; i < 5; i++) {
        tmp.days[i].minTemp = 127;
        tmp.days[i].maxTemp = -127;
        tmp.days[i].nightTemp = 127;
    }

    int cityTz = doc["city"]["timezone"] | 0;
    int baseDay = -1;

    int bestDayDiff[5];   memset(bestDayDiff, 99, sizeof(bestDayDiff));
    int bestNightDiff[5]; memset(bestNightDiff, 99, sizeof(bestNightDiff));

    for (JsonVariant v : list) {
        time_t t = (time_t)(v["dt"].as<long>() + cityTz);
        struct tm tm;
        gmtime_r(&t, &tm);

        if (baseDay < 0)
            baseDay = tm.tm_mday;

        int dayIdx = tm.tm_mday - baseDay;
        if (dayIdx < 0 || dayIdx >= 5) continue;

        if (tmp.days[dayIdx].date[0] == '\0') {
            strftime(tmp.days[dayIdx].date,
                     sizeof(tmp.days[dayIdx].date),
                     "%Y-%m-%d", &tm);
            if (tmp.count < dayIdx + 1)
                tmp.count = dayIdx + 1;
        }

        int8_t tMin = roundf(v["main"]["temp_min"].as<float>());
        int8_t tMax = roundf(v["main"]["temp_max"].as<float>());
        int8_t tCur = roundf(v["main"]["temp"].as<float>());

        uint16_t wid = v["weather"][0]["id"] | 804;

        if (tMin < tmp.days[dayIdx].minTemp) tmp.days[dayIdx].minTemp = tMin;
        if (tMax > tmp.days[dayIdx].maxTemp) tmp.days[dayIdx].maxTemp = tMax;

        int dayDiff = abs(tm.tm_hour - 15);
        if (dayDiff < bestDayDiff[dayIdx]) {
            bestDayDiff[dayIdx] = dayDiff;
            tmp.days[dayIdx].dayWeatherId = wid;
        }

        int nightDiff = abs(tm.tm_hour - 3);
        if (nightDiff < bestNightDiff[dayIdx]) {
            bestNightDiff[dayIdx] = nightDiff;
            tmp.days[dayIdx].nightWeatherId = wid;
            tmp.days[dayIdx].nightTemp = tCur;
        }
    }

    if (tmp.count == 0) {
        Serial.println("[Forecast] no valid days parsed");
        return false;
    }

    for (int i = 0; i < tmp.count; i++) {
        if (tmp.days[i].nightTemp == 127)
            tmp.days[i].nightTemp =
                (tmp.days[i].minTemp + tmp.days[i].maxTemp) / 2;
        if (tmp.days[i].dayWeatherId == 0)
            tmp.days[i].dayWeatherId = 804;
        if (tmp.days[i].nightWeatherId == 0)
            tmp.days[i].nightWeatherId = 804;
    }

    // ─── КОММИТ ДАННЫХ ───────────────────────────────
    _data = tmp;
    _data.valid = true;
    _data.ts = millis();
    return true;
}