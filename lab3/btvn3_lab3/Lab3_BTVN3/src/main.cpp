#include <Arduino.h>
#include <WiFi.h>
#include <Wire.h>
#include <time.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

const char* ssid = "Wokwi-GUEST";
const char* password = "";

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_ADDR 0x3C

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

const long GMT_OFFSET_SEC = 7 * 3600;
const int DAYLIGHT_OFFSET_SEC = 0;
const char* NTP_SERVER = "pool.ntp.org";

time_t savedEpoch = 0;
unsigned long savedMillis = 0;
bool ntpSynced = false;

void showClock(bool wifiOK, bool ntpOK)
{
    time_t now = savedEpoch;

    if (savedEpoch > 0) {
        unsigned long elapsed = millis() - savedMillis;
        now = savedEpoch + elapsed / 1000;
    }

    struct tm timeInfo;
    localtime_r(&now, &timeInfo);

    char timeText[16];
    char dateText[20];

    strftime(timeText, sizeof(timeText), "%H:%M:%S", &timeInfo);
    strftime(dateText, sizeof(dateText), "%d/%m/%Y", &timeInfo);

    display.clearDisplay();
    display.setTextColor(SSD1306_WHITE);

    display.setTextSize(2);
    display.setCursor(8, 2);
    display.println(timeText);

    display.setTextSize(1);
    display.setCursor(28, 27);
    display.println(dateText);

    display.drawLine(0, 40, 127, 40, SSD1306_WHITE);

    display.setCursor(2, 46);

    if (wifiOK)
        display.print("WiFi: OK");
    else
        display.print("WiFi: LOST");

    display.setCursor(76, 46);

    if (ntpOK)
        display.print("NTP:OK");
    else
        display.print("LOCAL");

    display.display();

    Serial.print("THOI GIAN: ");
    Serial.print(timeText);

    if (wifiOK)
        Serial.println("  [WiFi OK]");
    else
        Serial.println("  [CANH BAO: MAT WIFI]");
}

bool connectWiFi()
{
    Serial.println("Dang ket noi WiFi...");

    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);

    int retry = 0;

    while (WiFi.status() != WL_CONNECTED && retry < 20) {
        delay(500);
        Serial.print(".");
        retry++;
    }

    Serial.println();

    if (WiFi.status() == WL_CONNECTED) {
        Serial.println("WiFi da ket noi!");
        Serial.print("IP: ");
        Serial.println(WiFi.localIP());
        return true;
    }

    Serial.println("Khong ket noi duoc WiFi!");
    return false;
}

bool syncNTP()
{
    if (WiFi.status() != WL_CONNECTED)
        return false;

    Serial.println("Dang dong bo thoi gian tu NTP...");

    configTime(
        GMT_OFFSET_SEC,
        DAYLIGHT_OFFSET_SEC,
        NTP_SERVER
    );

    struct tm timeInfo;

    if (!getLocalTime(&timeInfo, 5000)) {
        Serial.println("NTP that bai!");
        return false;
    }

    time(&savedEpoch);
    savedMillis = millis();
    ntpSynced = true;

    Serial.println("Dong bo NTP thanh cong!");

    return true;
}

void setup()
{
    Serial.begin(115200);

    Wire.begin(21, 22);

    if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR)) {
        Serial.println("Khong tim thay OLED!");

        while (true) {
            delay(1000);
        }
    }

    display.clearDisplay();
    display.setTextColor(SSD1306_WHITE);
    display.setTextSize(1);

    display.setCursor(15, 20);
    display.println("SMART NTP CLOCK");

    display.setCursor(18, 35);
    display.println("Dang khoi dong...");

    display.display();

    Serial.println();
    Serial.println("==============================");
    Serial.println("   DONG HO THONG MINH NTP");
    Serial.println("==============================");

    connectWiFi();

    if (WiFi.status() == WL_CONNECTED)
        syncNTP();

    if (!ntpSynced) {
        savedEpoch = 0;
        savedMillis = millis();
    }
}

void loop()
{
    static unsigned long lastDisplay = 0;
    static unsigned long lastReconnect = 0;
    static unsigned long lastNTP = 0;

    // Hiển thị mỗi giây
    if (millis() - lastDisplay >= 1000) {

        lastDisplay = millis();

        showClock(
            WiFi.status() == WL_CONNECTED,
            ntpSynced
        );
    }

    // Mất WiFi -> tự kết nối lại
    if (WiFi.status() != WL_CONNECTED) {

        if (millis() - lastReconnect >= 5000) {

            lastReconnect = millis();

            Serial.println("Dang thu ket noi lai...");

            WiFi.disconnect();
            WiFi.begin(ssid, password);
        }
    }

    // WiFi trở lại -> đồng bộ NTP
    if (WiFi.status() == WL_CONNECTED) {

        if (!ntpSynced || millis() - lastNTP >= 60000) {

            lastNTP = millis();

            if (syncNTP()) {
                Serial.println("Da dong bo lai thoi gian NTP!");
            }
        }
    }

    delay(20);
}