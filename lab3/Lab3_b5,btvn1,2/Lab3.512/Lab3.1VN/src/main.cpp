#include <Arduino.h>
#include <WiFi.h>
#include <ESP32Ping.h>

// Thông tin mạng WiFi (Wokwi-GUEST mặc định cho giả lập)
const char* ssid = "Wokwi-GUEST";
const char* password = "";

// Cấu hình thời gian và giới hạn lỗi
const unsigned long PING_INTERVAL = 5 * 60 * 1000; // 5 phút = 300,000 ms
unsigned long lastPingTime = 0;

int failCount = 0;
const int MAX_FAILS = 3;

const char* targetHost = "8.8.8.8"; // IP mục tiêu để kiểm tra Internet

void setupWiFi() {
    Serial.println();
    Serial.print("Đang kết nối tới WiFi: ");
    Serial.println(ssid);

    WiFi.begin(ssid, password);

    unsigned long startAttemptTime = millis();
    // Chờ kết nối tối đa 15 giây
    while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < 15000) {
        delay(500);
        Serial.print(".");
    }

    if (WiFi.status() == WL_CONNECTED) {
        Serial.println("\nKết nối WiFi thành công!");
        Serial.print("Địa chỉ IP: ");
        Serial.println(WiFi.localIP());
    } else {
        Serial.println("\nKết nối WiFi thất bại! Thử lại sau...");
    }
}

void setup() {
    Serial.begin(115200);
    delay(1000);

    setupWiFi();
}

void loop() {
    // 1. Kiểm tra nhanh trạng thái kết nối phần cứng WiFi cục bộ
    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("Mất kết nối WiFi cục bộ! Đang kết nối lại...");
        setupWiFi();
        return;
    }

    unsigned long currentMillis = millis();

    // 2. Định kỳ mỗi 5 phút thực hiện kiểm tra Internet thông qua Ping
    if (currentMillis - lastPingTime >= PING_INTERVAL || lastPingTime == 0) {
        lastPingTime = currentMillis;

        Serial.println("----------------------------------------");
        Serial.println("Đang kiểm tra đường truyền Internet (Ping 8.8.8.8)...");

        // Thực hiện ping (gửi 3 gói tin)
        bool success = Ping.ping(targetHost, 3);

        if (success) {
            Serial.println("Ping thành công! Internet hoạt động bình thường.");
            failCount = 0; // Reset bộ đếm lỗi nếu thành công
        } else {
            failCount++;
            Serial.printf("Cảnh báo: Ping thất bại! Số lần lỗi liên tiếp: %d/%d\n", failCount, MAX_FAILS);

            // 3. Nếu lỗi liên tiếp quá 3 lần, tiến hành restart hệ thống
            if (failCount >= MAX_FAILS) {
                Serial.println("Lỗi Internet kéo dài! Tiến hành khởi động lại hệ thống (ESP.restart())...");
                delay(1000);
                ESP.restart();
            }
        }
        Serial.println("----------------------------------------");
    }
}