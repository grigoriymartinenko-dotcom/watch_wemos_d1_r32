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

private:
    DHT dht;

    float t_raw = NAN;
    float h_raw = NAN;

    float t_f = NAN;
    float h_f = NAN;

    static constexpr float K = 0.2f; // ПИФ
};