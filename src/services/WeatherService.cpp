#include "services/WeatherService.h"
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

static const char* OPENWEATHER_KEY = "07108cf067a5fdf5aa26dce75354400f";
static const char* CITY = "Kharkiv";
static const char* UNITS = "metric";
static const char* LANG  = "en";

static const unsigned long CACHE_MS = 15UL * 60UL * 1000UL;

void WeatherService::begin() {
    _lastFetch = 0;
    _data.valid = false;
}

const WeatherData& WeatherService::data() const {
    return _data;
}

bool WeatherService::update() {
    unsigned long now = millis();

    if (!WiFi.isConnected()) return false;
    if (_data.valid && (now - _lastFetch < CACHE_MS)) return false;

    bool ok = fetchFromApi();
    if (ok) _lastFetch = now;
    return ok;
}

bool WeatherService::fetchFromApi() {
    HTTPClient http;

    String url =
        String("http://api.openweathermap.org/data/2.5/weather?q=") +
        CITY + "&appid=" + OPENWEATHER_KEY +
        "&units=" + UNITS +
        "&lang=" + LANG;

    http.begin(url);
    int code = http.GET();
    if (code != HTTP_CODE_OK) {
        http.end();
        return false;
    }

    StaticJsonDocument<1024> doc;
    if (deserializeJson(doc, http.getStream())) {
        http.end();
        return false;
    }
    http.end();

    _data.temp = doc["main"]["temp"].as<float>();
    _data.feels = doc["main"]["feels_like"].as<float>();
    _data.weatherId = doc["weather"][0]["id"].as<uint16_t>();
    _data.desc = doc["weather"][0]["description"].as<String>();
    _data.valid = true;
    _data.ts = millis();

    return true;
}