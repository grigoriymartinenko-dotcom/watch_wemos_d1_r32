#pragma once
#include <Adafruit_ST7735.h>

/*
 * Screen (NEW API)
 * ================
 * Базовый интерфейс экранов.
 *
 * Логика:
 * - begin(): вызывается при активации экрана
 * - update(): вызывается каждый loop()
 * - onUp/onDown/onOk/onBack: кнопки
 * - drawButtons(): подписи кнопок (по умолчанию есть)
 */
class Screen {
public:
    virtual ~Screen() = default;

    virtual void begin() {}
    virtual void update() {}

    virtual void onUp() {}
    virtual void onDown() {}
    virtual void onOk() {}
    virtual void onBack() {}

    // Подписи кнопок внизу экрана (можно переопределять)
    virtual void drawButtons(Adafruit_ST7735& tft) {
        tft.setTextSize(1);
        tft.setTextColor(ST77XX_WHITE, ST77XX_BLACK);

        const int y = tft.height() - 10;
        tft.fillRect(0, y - 1, tft.width(), 11, ST77XX_BLACK);

        tft.setCursor(4, y);   tft.print("UP");
        tft.setCursor(40, y);  tft.print("OK");
        tft.setCursor(76, y);  tft.print("BACK");
        tft.setCursor(120, y); tft.print("DOWN");
    }
};
