#include "sensors/DHTSensor.h"

DHTSensor::DHTSensor(uint8_t pin, uint8_t type)
    : dht(pin, type) {}

void DHTSensor::begin() {
    dht.begin();
}

bool DHTSensor::update() {
    float t = dht.readTemperature();
    float h = dht.readHumidity();

    if (isnan(t) || isnan(h)) return false;

    _tRaw = t;
    _hRaw = h;

    if (isnan(_tF)) _tF = _tRaw;
    else            _tF += _kT * (_tRaw - _tF);

    if (isnan(_hF)) _hF = _hRaw;
    else            _hF += _kH * (_hRaw - _hF);

    return true;
}

float DHTSensor::temperature() const {
    return _tF;
}

float DHTSensor::humidity() const {
    return _hF;
}

void DHTSensor::setDayProfile() {
    _kT = K_T_DAY;
    _kH = K_H_DAY;
}

void DHTSensor::setNightProfile() {
    _kT = K_T_NIGHT;
    _kH = K_H_NIGHT;
}