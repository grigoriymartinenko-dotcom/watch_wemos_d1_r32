#pragma once

enum class NightModeState {
    AUTO,
    ON,
    OFF
};

struct NightMode {
    NightModeState state = NightModeState::AUTO;
    bool isNight = false;
};

extern NightMode nightMode;