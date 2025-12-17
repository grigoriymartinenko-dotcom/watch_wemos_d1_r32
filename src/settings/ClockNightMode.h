#pragma once
#include <stdint.h>

/*
 * ClockNightMode
 * ==============
 * Минимальный ночной режим:
 * - DAY: светлая тема
 * - NIGHT: тёмная тема
 * - AUTO: NIGHT в [22..6], иначе DAY
 */
enum class ClockNightMode : uint8_t {
    DAY = 0,
    NIGHT = 1,
    AUTO = 2
};

extern ClockNightMode clockNightMode;

// Хелпер: вычислить "ночь сейчас?"
inline bool isNightHour(uint8_t hour) {
    return (hour >= 22) || (hour < 7);
}
