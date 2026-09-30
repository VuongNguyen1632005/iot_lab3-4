#include <Arduino.h>
#include <WiFi.h>
#include "time.h"
#include "esp_sntp.h"

// Thông tin mạng WiFi (Wokwi-GUEST hỗ trợ kết nối Internet giả lập)
const char* ssid = "Wokwi-GUEST";
const char* password = "";

// Cấu hình NTP Server và Múi giờ
const char* ntpServer1 = "pool.ntp.org";
const char* ntpServer2 = "time.nist.gov";
const long  gmtOffset_sec = 7 * 3600;     // Múi giờ Việt Nam (GMT+7)
const int   daylightOffset_sec = 0;       // Không dùng giờ mùa hè

// Cấu hình chu kỳ đồng bộ lại (6 tiếng = 6 * 3600 * 1000 ms)
// Lưu ý: Khi test trên Wokwi, bạn có thể chỉnh ngắn lại (ví dụ: 30000 = 30 giây) để dễ quan sát
const unsigned long SYNC_INTERVAL = 6UL * 60UL * 60UL * 1000UL; 
unsigned long lastSyncTime = 0;

void printLocalTime() {
    struct tm timeinfo;
    if (!getLocalTime(&timeinfo)) {
        Serial.println("Không thể lấy thời gian hiện tại từ hệ thống!");
        return;
    }
    Serial.printf("Thời gian hiện tại: %02d/%02d/%d %02d:%02d:%02d\n",
                  timeinfo.tm_mday, timeinfo.tm_mon + 1, timeinfo.tm_year + 1900,
                  timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec);
}

void setupWiFi() {
    Serial.println();
    Serial.print("Đang kết nối tới WiFi: ");
    Serial.println(ssid);

    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\nKết nối WiFi thành công!");
}

void syncNTPTime() {
    Serial.println("\n----------------------------------------");
    Serial.println("Đang tiến hành đồng bộ lại thời gian từ NTP Server...");

    // Lấy mốc thời gian trước khi đồng bộ (Epoch time)
    time_t timeBeforeSync;
    time(&timeBeforeSync);

    // Khởi động lại dịch vụ SNTP để bắt buộc yêu cầu gói tin mới từ Server
    sntp_stop();
    sntp_setoperatingmode(SNTP_OPMODE_POLL);
    sntp_setservername(0, (char*)ntpServer1);
    sntp_setservername(1, (char*)ntpServer2);
    sntp_init();

    // Chờ thời gian được cập nhật (tối đa 10 giây)
    int retry = 0;
    while (sntp_get_sync_status() == SNTP_SYNC_STATUS_RESET && ++retry < 20) {
        delay(500);
        Serial.print(".");
    }
    Serial.println();

    // Lấy mốc thời gian sau khi đồng bộ
    time_t timeAfterSync;
    time(&timeAfterSync);

    // Tính độ chênh lệch (sai số)
    long drift = (long)(timeAfterSync - timeBeforeSync);
    
    Serial.printf("Log chênh lệch: Đồng hồ nội bộ lệch %ld giây so với NTP Server.\n", drift);
    printLocalTime();
    Serial.println("----------------------------------------");
}

void setup() {
    Serial.begin(115200);
    delay(1000);

    setupWiFi();

    // Khởi tạo cấu hình NTP lần đầu
    configTime(gmtOffset_sec, daylightOffset_sec, ntpServer1, ntpServer2);

    Serial.println("Đang chờ đồng bộ thời gian lần đầu từ NTP Server...");
    struct tm timeinfo;
    int timeout = 0;
    while (!getLocalTime(&timeinfo) && timeout < 20) {
        delay(1000);
        Serial.print(".");
        timeout++;
    }

    if (timeout < 20) {
        Serial.println("\nĐồng bộ thời gian lần đầu thành công!");
        printLocalTime();
    } else {
        Serial.println("\nQuá thời gian đồng bộ NTP lần đầu!");
    }

    lastSyncTime = millis();
}

void loop() {
    unsigned long currentMillis = millis();

    // Kiểm tra định kỳ mỗi 6 tiếng (hoặc theo cấu hình SYNC_INTERVAL)
    if (currentMillis - lastSyncTime >= SYNC_INTERVAL) {
        lastSyncTime = currentMillis;

        // Đảm bảo có kết nối mạng trước khi gọi đồng bộ
        if (WiFi.status() == WL_CONNECTED) {
            syncNTPTime();
        } else {
            Serial.println("Mất kết nối WiFi, bỏ qua chu kỳ đồng bộ NTP!");
            setupWiFi();
        }
    }
}