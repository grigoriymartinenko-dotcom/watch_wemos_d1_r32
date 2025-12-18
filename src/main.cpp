#include <Arduino.h>
#include <SPI.h>
#include <WiFi.h>

#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
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

/* ================= CORE ================= */
#include "core/Screen.h"
#include "core/ScreenManager.h"

/* ================= SERVICES ================= */
#include "services/ForecastService.h"

/* ================= SCREENS ================= */
#include "screens/ClockScreen.h"
#include "screens/ForecastScreen.h"

/* ================= UI ================= */
#include "ui/StatusBar.h"

/* ================= WIFI ================= */
const char* WIFI_SSID = "grig";
const char* WIFI_PASS = "magnetic";

/* ================= BUTTONS =================
 * BTN1 = UP    GPIO17
 * BTN2 = DOWN  GPIO16
 * BTN3 = OK    GPIO22
 * BTN4 = BACK  GPIO21
 */
#define BTN_UP     17
#define BTN_DOWN   16
#define BTN_OK     22
#define BTN_BACK   21

/* ================= GLOBAL OBJECTS ================= */
ScreenManager screenManager;
ForecastService forecast;
StatusBar statusBar;

/* ================= SCREENS ================= */
ClockScreen clockScreen(
    tft,
    rtc,
    screenManager
);

ForecastScreen forecastScreen(
    tft,
    forecast,
    screenManager,
    &clockScreen
);

/* ================= SETUP ================= */
void setup() {
    Serial.begin(115200);
    delay(200);

    /* --- Buttons --- */
    pinMode(BTN_UP,   INPUT_PULLUP);
    pinMode(BTN_DOWN, INPUT_PULLUP);
    pinMode(BTN_OK,   INPUT_PULLUP);
    pinMode(BTN_BACK, INPUT_PULLUP);

    /* --- TFT --- */
    tft.initR(INITR_BLACKTAB);
    tft.setRotation(1);
    tft.fillScreen(ST77XX_BLACK);

    /* --- RTC --- */
    rtc.Begin();

    /* --- StatusBar --- */
    statusBar.begin(tft);
    statusBar.setNtp(StatusColor::OK);

    /* --- WiFi --- */
    WiFi.mode(WIFI_STA);
    WiFi.begin(WIFI_SSID, WIFI_PASS);

    Serial.print("WiFi connecting");
    for (int i = 0; i < 20 && WiFi.status() != WL_CONNECTED; i++) {
        delay(300);
        Serial.print(".");
    }
    Serial.println();

    if (WiFi.isConnected()) {
        Serial.print("WiFi OK, IP: ");
        Serial.println(WiFi.localIP());
        statusBar.setWiFi(StatusColor::OK);
    } else {
        Serial.println("WiFi FAILED");
        statusBar.setWiFi(StatusColor::ERROR);
    }

    /* --- Forecast service --- */
    forecast.begin();
    if (forecast.update(true))
        statusBar.setApi(StatusColor::OK);
    else
        statusBar.setApi(StatusColor::ERROR);

    /* --- Link screens --- */
    clockScreen.setForecastScreen(&forecastScreen);

    /* --- Start screen --- */
    screenManager.set(&clockScreen);
}

/* ================= LOOP ================= */
void loop() {
    /* --- Buttons polling (debounce) --- */
    static uint32_t lastBtnMs = 0;
    if (millis() - lastBtnMs > 150) {
        lastBtnMs = millis();

        if (!digitalRead(BTN_UP))   screenManager.onUp();
        if (!digitalRead(BTN_DOWN)) screenManager.onDown();
        if (!digitalRead(BTN_OK))   screenManager.onOk();
        if (!digitalRead(BTN_BACK)) screenManager.onBack();
    }

    /* --- Services --- */
    if (forecast.update()) {
        statusBar.setApi(StatusColor::OK);
    }

    /* --- UI --- */
    statusBar.draw();
    screenManager.update();
}