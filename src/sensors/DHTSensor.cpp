#include "DHTSensor.h"

DHTSensor::DHTSensor(uint8_t pin, uint8_t type)
    : dht(pin, type) {}

void DHTSensor::begin() {
    dht.begin();
}

bool DHTSensor::update() {
    float t = dht.readTemperature();
    float h = dht.readHumidity();

    if (isnan(t) || isnan(h)) {
        return false;
    }

    t_raw = t;
    h_raw = h;

    // --- ПИФ ---
    if (isnan(t_f)) t_f = t_raw;
    else            t_f += K * (t_raw - t_f);

    if (isnan(h_f)) h_f = h_raw;
    else            h_f += K * (h_raw - h_f);

    return true;
}

float DHTSensor::temperature() const {
    return t_f;
}

float DHTSensor::humidity() const {
    return h_f;
}