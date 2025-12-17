#pragma once
#include <Arduino.h>
#include <DHT.h>

class DHTSensor {
public:
    DHTSensor(uint8_t pin, uint8_t type);

    void begin();
    bool update();

    float temperature() const;
    float humidity() const;

    // профили ПИФ
    void setDayProfile();
    void setNightProfile();

private:
    DHT dht;

    float _tRaw = NAN;
    float _hRaw = NAN;

    float _tF = NAN;
    float _hF = NAN;

    float _kT = 0.20f;
    float _kH = 0.15f;

    static constexpr float K_T_DAY   = 0.20f;
    static constexpr float K_H_DAY   = 0.15f;
    static constexpr float K_T_NIGHT = 0.10f;
    static constexpr float K_H_NIGHT = 0.08f;
};