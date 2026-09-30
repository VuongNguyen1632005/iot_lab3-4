#include <Arduino.h>
#include <WiFi.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "time.h"

// Cấu hình OLED
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Thông tin WiFi (Trên Wokwi, dùng "Wokwi-GUEST" không cần mật khẩu)
const char* ssid     = "Wokwi-GUEST";
const char* password = "";

// Cấu hình NTP Server (Múi giờ Việt Nam là UTC+7 = 25200 giây)
const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 7 * 3600;
const int   daylightOffset_sec = 0;

void setup() {
  Serial.begin(115200);

  // Khởi tạo màn hình OLED
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.print("Connecting to WiFi...");
  display.display();

  // Kết nối WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected!");

  // Cấu hình đồng bộ NTP
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
}

void loop() {
  struct tm timeinfo;
  display.clearDisplay();

  // Kiểm tra trạng thái WiFi và hiển thị biến trạng thái
  bool wifiConnected = (WiFi.status() == WL_CONNECTED);
  
  // Hiển thị trạng thái góc trên cùng bên phải
  display.setCursor(95, 0);
  if (wifiConnected) {
    display.print("[OK]");
  } else {
    display.print("[X]");
  }

  // Lấy thời gian hiện tại
  if (!getLocalTime(&timeinfo)) {
    display.setCursor(0, 20);
    display.print("Syncing NTP...");
  } else {
    char dateStr[20];
    char timeStr[20];

    // Định dạng dòng 1: Ngày/Tháng/Năm (DD/MM/YYYY)
    strftime(dateStr, sizeof(dateStr), "%d/%m/%Y", &timeinfo);
    
    // Định dạng dòng 2: Giờ/Phút/Giây (HH:MM:SS)
    strftime(timeStr, sizeof(timeStr), "%H:%M:%S", &timeinfo);

    // In lên OLED
    display.setCursor(0, 10);
    display.setTextSize(1);
    display.print("Date:");
    
    display.setCursor(0, 25);
    display.setTextSize(2);
    display.print(dateStr);

    display.setCursor(0, 48);
    display.setTextSize(1);
    display.print("Time: ");
    display.print(timeStr);
  }

  display.display();
  delay(1000); // Cập nhật mỗi giây
}