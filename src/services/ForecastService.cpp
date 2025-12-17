#include "services/ForecastService.h"
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
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
        memset(_data.days[i].date, 0, sizeof(_data.days[i].date));
        _data.days[i].minTemp = 127;
        _data.days[i].maxTemp = -127;
        _data.days[i].nightTemp = 127;
        _data.days[i].dayWeatherId = 0;
        _data.days[i].nightWeatherId = 0;
    }
}

bool ForecastService::parseDtTxt(const char* dt, char outDate[11], int& outHour) {
    // dt format: "YYYY-MM-DD HH:MM:SS"
    if (!dt || strlen(dt) < 13) return false;

    // date
    // copy first 10 chars
    for (int i = 0; i < 10; i++) outDate[i] = dt[i];
    outDate[10] = '\0';

    // hour at positions 11-12
    char hh[3] = { dt[11], dt[12], '\0' };
    outHour = atoi(hh);
    return true;
}

bool ForecastService::update(bool force) {
    unsigned long now = millis();

    if (!WiFi.isConnected()) return false;

    if (!force && _data.valid && (now - _lastFetch < CACHE_MS)) {
        return false; // кэш валиден
    }

    bool ok = fetchFromApi();
    if (ok) _lastFetch = now;
    return ok;
}

bool ForecastService::fetchFromApi() {
    HTTPClient http;

    String url =
        String("http://api.openweathermap.org/data/2.5/forecast?q=") +
        CITY + "&appid=" + OPENWEATHER_KEY +
        "&units=" + UNITS +
        "&lang=" + LANG;

    http.begin(url);
    int code = http.GET();
    if (code != HTTP_CODE_OK) {
        http.end();
        return false;
    }

    // ⬆️ УВЕЛИЧЕН БУФЕР (КЛЮЧЕВО!)
    DynamicJsonDocument doc(48 * 1024);

    DeserializationError err = deserializeJson(doc, http.getStream());
    http.end();

    if (err) {
        Serial.print("Forecast JSON error: ");
        Serial.println(err.c_str());
        return false;
    }
    else {Serial.println("Forecast good: ");}

    resetData();

    JsonArray list = doc["list"].as<JsonArray>();
    if (list.isNull()) return false;

    int bestDayDiff[5];   for (int i=0;i<5;i++) bestDayDiff[i] = 99;
    int bestNightDiff[5]; for (int i=0;i<5;i++) bestNightDiff[i] = 99;

    for (JsonVariant v : list) {
        const char* dt = v["dt_txt"] | nullptr;

        char dateStr[11];
        int hour = 0;
        if (!parseDtTxt(dt, dateStr, hour)) continue;

        int dayIdx = -1;
        for (int i = 0; i < _data.count; i++) {
            if (strcmp(_data.days[i].date, dateStr) == 0) {
                dayIdx = i;
                break;
            }
        }
        if (dayIdx == -1) {
            if (_data.count >= 5) continue;
            dayIdx = _data.count++;
            strncpy(_data.days[dayIdx].date, dateStr, 11);
        }

        int8_t tMin = round(v["main"]["temp_min"].as<float>());
        int8_t tMax = round(v["main"]["temp_max"].as<float>());
        int8_t tCur = round(v["main"]["temp"].as<float>());

        uint16_t wid = v["weather"][0]["id"] | 804;

        if (tMin < _data.days[dayIdx].minTemp) _data.days[dayIdx].minTemp = tMin;
        if (tMax > _data.days[dayIdx].maxTemp) _data.days[dayIdx].maxTemp = tMax;

        int dayDiff = abs(hour - 15);
        if (dayDiff < bestDayDiff[dayIdx]) {
            bestDayDiff[dayIdx] = dayDiff;
            _data.days[dayIdx].dayWeatherId = wid;
        }

        int nightDiff = abs(hour - 3);
        if (nightDiff < bestNightDiff[dayIdx]) {
            bestNightDiff[dayIdx] = nightDiff;
            _data.days[dayIdx].nightWeatherId = wid;
            _data.days[dayIdx].nightTemp = tCur;
        }
    }

    for (int i = 0; i < _data.count; i++) {
        if (_data.days[i].nightTemp == 127)
            _data.days[i].nightTemp = (_data.days[i].minTemp + _data.days[i].maxTemp) / 2;

        if (_data.days[i].dayWeatherId == 0)   _data.days[i].dayWeatherId = 804;
        if (_data.days[i].nightWeatherId == 0) _data.days[i].nightWeatherId = 804;
    }

    _data.valid = (_data.count > 0);
    _data.ts = millis();
    return _data.valid;
}