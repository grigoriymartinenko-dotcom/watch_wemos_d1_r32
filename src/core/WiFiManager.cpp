#include "core/WiFiManager.h"
#include <WiFi.h>

void WiFiManager::begin(const char* ssid1, const char* pass1,
                        const char* ssid2, const char* pass2) {
    _ssid1 = ssid1;
    _pass1 = pass1;
    _ssid2 = ssid2;
    _pass2 = pass2;

    WiFi.mode(WIFI_STA);
    WiFi.setAutoReconnect(true);
    WiFi.persistent(false);

    _state = State::IDLE;

    // стартуем сразу
    startPrimary();
}

bool WiFiManager::isConnected() const {
    return WiFi.isConnected();
}

const char* WiFiManager::stateText() const {
    switch (_state) {
        case State::IDLE:                return "IDLE";
        case State::CONNECTING_PRIMARY:  return "CONN_1";
        case State::CONNECTING_FALLBACK: return "CONN_2";
        case State::CONNECTED:           return "OK";
        case State::WAIT_RETRY:          return "RETRY";
        default:                         return "?";
    }
}

void WiFiManager::startPrimary() {
    if (!_ssid1 || !_ssid1[0]) {
        scheduleRetry();
        return;
    }

    WiFi.disconnect(true);
    delay(10);

    WiFi.begin(_ssid1, _pass1 ? _pass1 : "");
    _attemptStarted = millis();
    _state = State::CONNECTING_PRIMARY;
}

void WiFiManager::startFallback() {
    if (!_ssid2 || !_ssid2[0]) {
        scheduleRetry();
        return;
    }

    WiFi.disconnect(true);
    delay(10);

    WiFi.begin(_ssid2, _pass2 ? _pass2 : "");
    _attemptStarted = millis();
    _state = State::CONNECTING_FALLBACK;
}

void WiFiManager::scheduleRetry() {
    _nextRetryAt = millis() + RETRY_DELAY_MS;
    _state = State::WAIT_RETRY;
}

void WiFiManager::update() {
    unsigned long now = millis();

    // Если уже подключились — держим состояние CONNECTED
    if (WiFi.isConnected()) {
        _state = State::CONNECTED;
        return;
    }

    switch (_state) {
        case State::CONNECTING_PRIMARY:
            if (now - _attemptStarted > CONNECT_TIMEOUT_MS) {
                // пробуем fallback
                startFallback();
            }
            break;

        case State::CONNECTING_FALLBACK:
            if (now - _attemptStarted > CONNECT_TIMEOUT_MS) {
                scheduleRetry();
            }
            break;

        case State::WAIT_RETRY:
            if (now >= _nextRetryAt) {
                startPrimary();
            }
            break;

        case State::CONNECTED:
            // сюда попадём если WiFi отвалится — тогда пойдём в retry
            scheduleRetry();
            break;

        case State::IDLE:
        default:
            startPrimary();
            break;
    }
}