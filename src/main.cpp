#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>

#include "core/ScreenManager.h"
#include "screens/ClockScreen.h"
#include "screens/WeatherScreen.h"
#include "screens/SettingsScreen.h"

#include "sensors/DHTSensor.h"
#include <RtcDS1302.h>

/* ================= TFT ================= */
#define TFT_CS   5
#define TFT_DC   2
#define TFT_RST  4

Adafruit_ST7735 tft(TFT_CS, TFT_DC, TFT_RST);

/* ================= RTC ================= */
#define RTC_CLK 14
#define RTC_DAT 27
#define RTC_RST 26

ThreeWire myWire(RTC_DAT, RTC_CLK, RTC_RST);
RtcDS1302<ThreeWire> rtc(myWire);

/* ================= DHT ================= */
#define DHT_PIN   25
#define DHT_TYPE  DHT11

DHTSensor dht(DHT_PIN, DHT_TYPE);
unsigned long lastDhtRead = 0;

/* ================= MANAGER ================= */
ScreenManager screenManager;

/* ================= SCREENS ================= */
ClockScreen    clockScreen(tft, rtc, screenManager);
WeatherScreen  weatherScreen(tft, screenManager);
SettingsScreen settingsScreen(tft, screenManager);

/* ================= SETUP ================= */
void setup() {
    Serial.begin(115200);

    tft.initR(INITR_BLACKTAB);
    tft.setRotation(1);
    tft.fillScreen(ST77XX_BLACK);

    rtc.Begin();
    dht.begin();
    screenManager.set(&clockScreen);
}

/* ================= LOOP ================= */
void loop() {

    // --- DHT11 + ПИФ (раз в 3 сек) ---
    if (millis() - lastDhtRead >= 3000) {
        lastDhtRead = millis();
        dht.update();
    }
Serial.printf("T=%dC  H=%d%%\n",
    (int)dht.temperature(),
    (int)dht.humidity()
);
    screenManager.update();
}