#pragma once
#include <Arduino.h>

struct WeatherData {
    bool valid = false;
    float temp = 0;
    float feels = 0;
    uint16_t weatherId = 0;   // <-- ВАЖНО
    String desc;
    unsigned long ts = 0;
};

class WeatherService {
public:
    void begin();
    bool update();
    const WeatherData& data() const;

private:
    WeatherData _data;
    unsigned long _lastFetch = 0;

    bool fetchFromApi();
};