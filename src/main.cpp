#include <Arduino.h>
#include <Adafruit_ST7735.h>
#include <RtcDS1302.h>

#include "core/ScreenManager.h"
#include "core/NightMode.h"
#include "core/WiFiManager.h"

#include "screens/ClockScreen.h"
#include "screens/WeatherScreen.h"
#include "screens/SettingsScreen.h"

#include "sensors/DHTSensor.h"
#include "services/WeatherService.h"

/* ================= BUTTONS ================= */
#define BTN_UP     17
#define BTN_DOWN   16
#define BTN_OK     22
#define BTN_BACK   21

/* ================= WIFI CREDS ================= */
static const char* WIFI_SSID_1 = "grig";
static const char* WIFI_PASS_1 = "magnetic";          // <-- впиши пароль сюда
static const char* WIFI_SSID_2 = "gr";        // fallback (если нужно)
static const char* WIFI_PASS_2 = "magnetic";          // <-- и тут (если нужно)

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
WiFiManager wifiManager;

/* ================= SCREENS ================= */
ClockScreen    clockScreen(tft, rtc, screenManager);
WeatherScreen  weatherScreen(tft, screenManager);
SettingsScreen settingsScreen(tft, screenManager);

/* ================= SENSORS & SERVICES ================= */
DHTSensor dht(25, DHT11);
WeatherService weather;

/* ================= TIMERS ================= */
unsigned long lastDhtRead = 0;
static const unsigned long DHT_INTERVAL_MS = 60000;
static bool lastNight = false;

/* ================= BUTTON STATE ================= */
bool prevUp = HIGH, prevDown = HIGH, prevOk = HIGH, prevBack = HIGH;

void setup() {
    pinMode(BTN_UP,   INPUT_PULLUP);
    pinMode(BTN_DOWN, INPUT_PULLUP);
    pinMode(BTN_OK,   INPUT_PULLUP);
    pinMode(BTN_BACK, INPUT_PULLUP);

    tft.initR(INITR_BLACKTAB);
    tft.setRotation(1);
    tft.fillScreen(ST77XX_BLACK);

    rtc.Begin();

    dht.begin();
    weather.begin();

    // Wi-Fi manager (фоновые попытки подключения)
    wifiManager.begin(WIFI_SSID_1, WIFI_PASS_1, WIFI_SSID_2, WIFI_PASS_2);

    clockScreen.setLinks(&weatherScreen, &settingsScreen);
    settingsScreen.setClock(&clockScreen);
    weatherScreen.setClock(&clockScreen);

    screenManager.set(&clockScreen);
}

void loop() {
    unsigned long nowMs = millis();

    // Wi-Fi (не блокирует, просто обслуживает состояние)
    wifiManager.update();

    // DHT
    if (nowMs - lastDhtRead >= DHT_INTERVAL_MS) {
        lastDhtRead = nowMs;
        dht.update();
    }

    // PIF profile
    if (nightMode.isNight != lastNight) {
        nightMode.isNight ? dht.setNightProfile()
                          : dht.setDayProfile();
        lastNight = nightMode.isNight;
    }

    // Buttons (edge detect)
    bool up    = digitalRead(BTN_UP);
    bool down  = digitalRead(BTN_DOWN);
    bool ok    = digitalRead(BTN_OK);
    bool back  = digitalRead(BTN_BACK);

    if (prevUp == HIGH && up == LOW)       screenManager.onUp();
    if (prevDown == HIGH && down == LOW)  screenManager.onDown();
    if (prevOk == HIGH && ok == LOW)      screenManager.onOk();
    if (prevBack == HIGH && back == LOW)  screenManager.onBack();

    prevUp = up; prevDown = down; prevOk = ok; prevBack = back;

    screenManager.update();
}