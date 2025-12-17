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

    // --- управление ПИФ ---
    void setTempK(float k) { _kT = constrain(k, 0.01f, 1.0f); }
    void setHumK (float k) { _kH = constrain(k, 0.01f, 1.0f); }

    float tempK() const { return _kT; }
    float humK () const { return _kH; }

private:
    DHT dht;

    float _tRaw = NAN;
    float _hRaw = NAN;

    float _tF = NAN;
    float _hF = NAN;

    // коэффициенты ПИФ
    float _kT = 0.20f; // температура
    float _kH = 0.15f; // влажность (чуть плавнее)
};