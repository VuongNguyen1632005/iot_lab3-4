#include <Arduino.h>
#include <DHT.h>
#include <HTTPClient.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>

// Sửa URL Webhook.site ở đây. Bài 1 và Bài 3 dùng cùng URL này.
const char* WEBHOOK_URL = "https://webhook.site/d46f3a6d-de17-4679-a649-ef20172f80c5";

const char* WIFI_SSID = "Wokwi-GUEST";
const char* WIFI_PASSWORD = "";
const int DHT_PIN = 23;
const int SEND_EVERY_MS = 10000;

DHT dht(DHT_PIN, DHT22);

bool connectWiFi() {
  Serial.print("WiFi: Connecting");
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD, 6);

  // Chờ tối đa 20 giây để kết nối Wi-Fi.
  for (int i = 0; i < 80 && WiFi.status() != WL_CONNECTED; i++) {
    delay(250);
    Serial.print(".");
  }
  Serial.println();

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("WiFi: Connected");
    return true;
  }

  Serial.println("WiFi connection failed");
  return false;
}

void sendSensorData() {
  const float temperature = dht.readTemperature();
  const float humidity = dht.readHumidity();

  if (isnan(temperature) || isnan(humidity)) {
    Serial.println("DHT22 read failed");
    return;
  }

  String json = "{\"device_id\":\"esp32_dht22_01\",\"temperature\":";
  json += String(temperature, 1);
  json += ",\"humidity\":";
  json += String(humidity, 1);
  json += "}";

  Serial.println("================================");
  Serial.printf("Temperature: %.1f °C\n", temperature);
  Serial.printf("Humidity: %.1f %%\n", humidity);
  Serial.println("JSON:");
  Serial.println(json);
  Serial.println("Sending POST...");

  WiFiClientSecure secureClient;
  secureClient.setInsecure();
  HTTPClient http;
  http.setTimeout(10000);

  if (!http.begin(secureClient, WEBHOOK_URL)) {
    Serial.println("HTTP error: invalid Webhook URL");
    Serial.println("================================");
    return;
  }

  http.addHeader("Content-Type", "application/json");
  const int status = http.POST(json);
  if (status > 0) {
    Serial.printf("HTTP Status: %d\n", status);
    if (status >= 200 && status < 300) {
      Serial.println("Send successfully");
    } else {
      Serial.printf("HTTP error: server returned %d\n", status);
    }
  } else {
    Serial.printf("HTTP error: %s\n", http.errorToString(status).c_str());
  }
  http.end();
  Serial.println("================================");
}

void setup() {
  Serial.begin(115200);
  dht.begin();
  connectWiFi();
}

void loop() {
  if (WiFi.status() != WL_CONNECTED) {
    connectWiFi();
  }
  if (WiFi.status() == WL_CONNECTED) {
    sendSensorData();
  }
  delay(SEND_EVERY_MS);
}
