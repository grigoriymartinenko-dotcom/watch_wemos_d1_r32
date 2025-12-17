#include <Arduino.h>
#include <Adafruit_ST7735.h>
#include <RtcDS1302.h>

#include "core/ScreenManager.h"
#include "core/NightMode.h"
#include "screens/ClockScreen.h"
#include "screens/WeatherScreen.h"
#include "screens/SettingsScreen.h"
#include "sensors/DHTSensor.h"

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

/* ================= CORE ================= */
ScreenManager screenManager;

/* ================= SCREENS ================= */
ClockScreen    clockScreen(tft, rtc, screenManager);
WeatherScreen  weatherScreen(tft, screenManager);
SettingsScreen settingsScreen(tft, screenManager);

/* ================= DHT ================= */
DHTSensor dht(25, DHT11);

/* ================= TIMERS ================= */
unsigned long lastDhtRead = 0;
static const unsigned long DHT_INTERVAL_MS = 60000;
static bool lastNight = false;

void setup() {
    // --- TFT ---
    tft.initR(INITR_BLACKTAB);
    tft.setRotation(1);
    tft.fillScreen(ST77XX_BLACK);

    // --- RTC ---
    rtc.Begin();

    // --- DHT ---
    dht.begin();


    // --- связываем экраны ---
clockScreen.setLinks(&weatherScreen, &settingsScreen);
settingsScreen.setClock(&clockScreen);

screenManager.set(&clockScreen);   // стартовый экран
    // --- АКТИВНЫЙ ЭКРАН (КРИТИЧНО) ---
    //screenManager.set(&clockScreen);
}

void loop() {
    unsigned long nowMs = millis();

    if (nowMs - lastDhtRead >= DHT_INTERVAL_MS) {
        lastDhtRead = nowMs;
        dht.update();
    }

    // --- ПИФ профили ---
    if (nightMode.isNight != lastNight) {
        nightMode.isNight ? dht.setNightProfile()
                          : dht.setDayProfile();
        lastNight = nightMode.isNight;
    }

    screenManager.update();
}