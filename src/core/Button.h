#pragma once
#include <Arduino.h>

/*
 * Button
 * ======
 * Лёгкая кнопка с антидребезгом (без delay).
 *
 * ВАЖНО:
 * - Используем INPUT_PULLUP (кнопка на землю)
 * - pressed() возвращает true РОВНО один раз на нажатие
 */
class Button {
public:
    explicit Button(uint8_t pin, uint16_t debounce_ms = 200)
        : _pin(pin), _debounceMs(debounce_ms) {}

    void begin() {
        pinMode(_pin, INPUT_PULLUP);
        _lastState = raw();
        _lastEdgeMs = millis();
    }

    // "Событие": нажатие
    bool pressed() {
        const bool s = raw();
        const unsigned long now = millis();

        // Стабилизация входа
        if (s != _lastState) {
            _lastEdgeMs = now;
            _lastState = s;
        }

        // Считаем "нажатым" только стабильный LOW (pullup)
        if (now - _lastEdgeMs >= _debounceMs) {
            if (_lastStable != s) {
                _lastStable = s;
                if (_lastStable == true) { // true == pressed (LOW)
                    return true;
                }
            }
        }
        return false;
    }

private:
    // raw() -> true если кнопка нажата (LOW при INPUT_PULLUP)
    bool raw() const { return digitalRead(_pin) == LOW; }

    uint8_t _pin;
    uint16_t _debounceMs;

    bool _lastState = false;
    bool _lastStable = false;
    unsigned long _lastEdgeMs = 0;
};
