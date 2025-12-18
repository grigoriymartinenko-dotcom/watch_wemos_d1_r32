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
    void setNtp(StatusColor s);

    // дата для центра
    void setDate(uint8_t day, uint8_t month);

    void draw();

private:
    Adafruit_ST7735* _tft = nullptr;
    bool _dirty = true;
    StatusColor _wifi = StatusColor::ERROR;
    StatusColor _api  = StatusColor::ERROR;
    StatusColor _ntp  = StatusColor::ERROR;

    uint8_t _day = 0;
    uint8_t _month = 0;

    uint16_t color(StatusColor s);
};