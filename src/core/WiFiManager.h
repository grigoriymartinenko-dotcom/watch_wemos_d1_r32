#pragma once
#include <Arduino.h>

class WiFiManager {
public:
    enum class State {
        IDLE,
        CONNECTING_PRIMARY,
        CONNECTING_FALLBACK,
        CONNECTED,
        WAIT_RETRY
    };

    void begin(const char* ssid1, const char* pass1,
               const char* ssid2 = nullptr, const char* pass2 = nullptr);

    void update();                 // вызывать в loop()
    bool isConnected() const;
    State state() const { return _state; }

    const char* stateText() const; // для отладки/индикации

private:
    const char* _ssid1 = nullptr;
    const char* _pass1 = nullptr;
    const char* _ssid2 = nullptr;
    const char* _pass2 = nullptr;

    State _state = State::IDLE;

    unsigned long _attemptStarted = 0;
    unsigned long _nextRetryAt = 0;

    // Тайминги (аккуратные)
    static constexpr unsigned long CONNECT_TIMEOUT_MS = 15000; // 15 сек на попытку
    static constexpr unsigned long RETRY_DELAY_MS     = 5000;  // 5 сек пауза перед новой попыткой

    void startPrimary();
    void startFallback();
    void scheduleRetry();
};