#pragma once
#include <Arduino.h>

// 🌡 Thermometer 8x8
const uint8_t ICON_TEMP[] PROGMEM = {
  0b00100000,
  0b01110000,
  0b01110000,
  0b01110000,
  0b01110000,
  0b01110000,
  0b11111000,
  0b01110000
};

// 💧 Droplet 8x8
const uint8_t ICON_HUM[] PROGMEM = {
  0b00100000,
  0b01110000,
  0b01110000,
  0b11111000,
  0b11111000,
  0b11111000,
  0b01110000,
  0b00100000
};