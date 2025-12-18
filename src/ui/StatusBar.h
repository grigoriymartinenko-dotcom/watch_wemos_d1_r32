#pragma once
#include <Adafruit_ST7735.h>

enum class StatusColor {
    OK,
    WARN,
    ERROR
};

class StatusBar {
public:
    void begin(Adafruit_ST7735& tft);
    void setWiFi(StatusColor s);
    void setApi(StatusColor s);
    void setRtc(StatusColor s);
    void draw();

private:
    Adafruit_ST7735* _tft = nullptr;
    StatusColor _wifi = StatusColor::ERROR;
    StatusColor _api  = StatusColor::ERROR;
    StatusColor _rtc  = StatusColor::ERROR;

    uint16_t color(StatusColor s);
};