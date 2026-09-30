#include <Arduino.h>
#include <WiFi.h>

const char* ssid = "Wokwi-GUEST";

int retryCount = 0;
const int MAX_RETRY = 5;
bool isConnected = false;

// Hàm bắt sự kiện WiFi
void WiFiEvent(WiFiEvent_t event) {
  switch (event) {
    case ARDUINO_EVENT_WIFI_STA_GOT_IP:
      Serial.println("\n[SUCCESS] DA KET NOI WIFI THANH CONG!");
      Serial.print("-> Dia chi IP: ");
      Serial.println(WiFi.localIP());
      isConnected = true;
      retryCount = 0;
      break;

    case ARDUINO_EVENT_WIFI_STA_DISCONNECTED:
      isConnected = false;
      Serial.println("\n[WARNING] BI MAT KET NOI WIFI!");

      if (retryCount < MAX_RETRY) {
        retryCount++;
        Serial.printf("-> Dang thu ket noi lai (Lan %d/%d)...\n", retryCount, MAX_RETRY);
        WiFi.begin(ssid);
      } else {
        Serial.println("-> Da vuot qua so lan retry max!");
      }
      break;

    default:
      break;
  }
}

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("\n==========================================");
  Serial.println("=== BAI 2: DEMO MAT NET & RECONNECT ===");
  Serial.println("==========================================");

  WiFi.onEvent(WiFiEvent);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid);
}

void loop() {
  static unsigned long lastTime = 0;

  // Cứ sau mỗi 8 giây kết nối thành công, chủ động ngắt WiFi để giả lập sự cố
  if (isConnected && (millis() - lastTime > 8000)) {
    lastTime = millis();
    Serial.println("\n------------------------------------------");
    Serial.println("[GIA LAP SU CO] Chu dong ngat WiFi...");
    Serial.println("------------------------------------------");
    WiFi.disconnect(); // Ngắt mạng để kích hoạt tự động kết nối lại
  }

  delay(500);
}