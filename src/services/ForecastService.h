#pragma once
#include <Arduino.h>

struct ForecastDay {
    char date[11];              // YYYY-MM-DD
    int8_t minTemp;
    int8_t maxTemp;
    int8_t nightTemp;
    uint16_t dayWeatherId;
    uint16_t nightWeatherId;
};

struct ForecastData {
    bool valid;
    uint8_t count;
    ForecastDay days[5];
    unsigned long ts;
};

class ForecastService {
public:
    void begin();
    bool update(bool force = false);
    const ForecastData& data() const;

private:
    ForecastData _data;
    unsigned long _lastFetch;

    static constexpr unsigned long CACHE_MS =
        60UL * 60UL * 1000UL; // 1 час

    bool fetchFromApi();
    void resetData();
};