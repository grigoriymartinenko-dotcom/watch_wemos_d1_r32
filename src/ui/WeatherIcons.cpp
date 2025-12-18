#include "ui/WeatherIcons.h"
#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>

static void drawSun(Adafruit_ST7735& tft, int x, int y) {
    tft.fillCircle(x, y, 6, ST77XX_YELLOW);
}

static void drawCloud(Adafruit_ST7735& tft, int x, int y) {
    tft.fillCircle(x - 4, y, 4, ST77XX_WHITE);
    tft.fillCircle(x + 2, y - 2, 5, ST77XX_WHITE);
    tft.fillRect(x - 8, y, 16, 6, ST77XX_WHITE);
}

static void drawRain(Adafruit_ST7735& tft, int x, int y) {
    drawCloud(tft, x, y);
    tft.drawLine(x - 4, y + 8, x - 6, y + 12, ST77XX_CYAN);
    tft.drawLine(x + 2, y + 8, x,     y + 12, ST77XX_CYAN);
}

static void drawStorm(Adafruit_ST7735& tft, int x, int y) {
    drawCloud(tft, x, y);
    tft.drawLine(x, y + 6, x - 3, y + 12, ST77XX_YELLOW);
    tft.drawLine(x - 3, y + 12, x + 3, y + 12, ST77XX_YELLOW);
}

static void drawSnow(Adafruit_ST7735& tft, int x, int y) {
    drawCloud(tft, x, y);
    tft.drawPixel(x - 4, y + 10, ST77XX_WHITE);
    tft.drawPixel(x,     y + 12, ST77XX_WHITE);
    tft.drawPixel(x + 4, y + 10, ST77XX_WHITE);
}

static void drawFog(Adafruit_ST7735& tft, int x, int y) {
    tft.drawFastHLine(x - 8, y,     16, ST77XX_WHITE);
    tft.drawFastHLine(x - 8, y + 4, 16, ST77XX_WHITE);
}

void drawWeatherIcon(Adafruit_ST7735& tft, int x, int y, WeatherIcon icon) {
    switch (icon) {
        case WeatherIcon::CLEAR:  drawSun(tft, x, y);  break;
        case WeatherIcon::CLOUDS: drawCloud(tft, x, y); break;
        case WeatherIcon::RAIN:   drawRain(tft, x, y); break;
        case WeatherIcon::STORM:  drawStorm(tft, x, y); break;
        case WeatherIcon::SNOW:   drawSnow(tft, x, y); break;
        case WeatherIcon::FOG:    drawFog(tft, x, y); break;
        default:                  drawCloud(tft, x, y); break;
    }
}